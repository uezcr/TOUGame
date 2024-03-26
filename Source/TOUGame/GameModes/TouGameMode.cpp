// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouGameMode.h"
#include "AssetRegistry/AssetData.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TouLogChannels.h"
#include "Misc/CommandLine.h"
#include "System/TouAssetManager.h"
#include "TouGameState.h"
#include "System/TouGameSession.h"
#include "Player/TouPlayerController.h"
#include "Player/TouPlayerState.h"
#include "Character/TouCharacter.h"
#include "UI/TouHUD.h"
#include "Character/TouPawnExtensionComponent.h"
#include "Character/TouPawnData.h"
#include "GameModes/TouWorldSettings.h"
#include "GameModes/TouExperienceDefinition.h"
#include "GameModes/TouExperienceManagerComponent.h"
#include "GameModes/TouUserFacingExperienceDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Development/TouDeveloperSettings.h"
#include "Player/TouPlayerSpawningManagerComponent.h"
#include "CommonUserSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "TimerManager.h"
#include "GameMapsSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouGameMode)

ATouGameMode::ATouGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ATouGameState::StaticClass();
	GameSessionClass = ATouGameSession::StaticClass();
	PlayerControllerClass = ATouPlayerController::StaticClass();
	ReplaySpectatorPlayerControllerClass = ATouReplayPlayerController::StaticClass();
	PlayerStateClass = ATouPlayerState::StaticClass();
	DefaultPawnClass = ATouCharacter::StaticClass();
	HUDClass = ATouHUD::StaticClass();
}

const UTouPawnData* ATouGameMode::GetPawnDataForController(const AController* InController) const
{
	// 查看玩家状态中是否已经设置了棋子数据
	if (InController != nullptr)
	{
		if (const ATouPlayerState* TouPS = InController->GetPlayerState<ATouPlayerState>())
		{
			if (const UTouPawnData* PawnData = TouPS->GetPawnData<UTouPawnData>())
			{
				return PawnData;
			}
		}
	}

	// 如果没有，则返回当前体验的默认值
	check(GameState);
	UTouExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UTouExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const UTouExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// 经验已加载，但仍没有棋子数据，暂时返回默认值
		return UTouAssetManager::Get().GetDefaultPawnData();
	}

	// 经验尚未加载，因此没有棋子数据
	return nullptr;
}

void ATouGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 等待下一帧，以便有时间初始化启动设置
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ATouGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// 优先顺序（最高者获胜）
	// - 匹配分配（如果存在）
	// - URL 选项覆盖
	// - 开发人员设置（仅限 PIE）
	// - 命令行覆盖
	// - 世界设置
	// - 专用服务器
	// - 默认体验

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UTouExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceId = GetDefault<UTouDeveloperSettings>()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}

	// 看看命令行是否希望设置体验
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UTouExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// 看看世界设置是否有默认体验
	if (!ExperienceId.IsValid())
	{
		if (ATouWorldSettings* TypedWorldSettings = Cast<ATouWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	UTouAssetManager& AssetManager = UTouAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogTouExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	// 最终退回到默认体验
	if (!ExperienceId.IsValid())
	{
		if (TryDedicatedServerLogin())
		{
			// 这将开始作为专用服务器托管
			return;
		}

		//@TODO: 从配置设置或其他设置中提取
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("TouExperienceDefinition"), FName("B_TouDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

bool ATouGameMode::TryDedicatedServerLogin()
{
	// 注册为活动专用服务器的一些基本代码，这将由游戏进行大量修改
	FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance && World && World->GetNetMode() == NM_DedicatedServer && World->URL.Map == DefaultMap)
	{
		// 只有当这是专用服务器上的默认地图时才会注册
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();

		// 专用服务器可能需要进行在线登录
		UserSubsystem->OnUserInitializeComplete.AddDynamic(this, &ATouGameMode::OnUserInitializedForDedicatedServer);

		// 专用服务器上没有本地用户，但索引 0 表示默认平台用户，由在线登录代码处理
		if (!UserSubsystem->TryToLoginForOnlinePlay(0))
		{
			OnUserInitializedForDedicatedServer(nullptr, false, FText(), ECommonUserPrivilege::CanPlayOnline, ECommonUserOnlineContext::Default);
		}

		return true;
	}

	return false;
}

void ATouGameMode::HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode)
{
	FPrimaryAssetType UserExperienceType = UTouUserFacingExperienceDefinition::StaticClass()->GetFName();
	
	// 找出要加载的 UserFacingExperience
	FPrimaryAssetId UserExperienceId;
	FString UserExperienceFromCommandLine;
	if (FParse::Value(FCommandLine::Get(), TEXT("UserExperience="), UserExperienceFromCommandLine) ||
		FParse::Value(FCommandLine::Get(), TEXT("Playlist="), UserExperienceFromCommandLine))
	{
		UserExperienceId = FPrimaryAssetId::ParseTypeAndName(UserExperienceFromCommandLine);
		if (!UserExperienceId.PrimaryAssetType.IsValid())
		{
			UserExperienceId = FPrimaryAssetId(FPrimaryAssetType(UserExperienceType), FName(*UserExperienceFromCommandLine));
		}
	}

	// 搜索匹配体验，强制加载即可，因为我们是在专用服务器上启动的
	UTouAssetManager& AssetManager = UTouAssetManager::Get();
	TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssetsWithType(UserExperienceType);
	if (ensure(Handle.IsValid()))
	{
		Handle->WaitUntilComplete();
	}

	TArray<UObject*> UserExperiences;
	AssetManager.GetPrimaryAssetObjectList(UserExperienceType, UserExperiences);
	UTouUserFacingExperienceDefinition* FoundExperience = nullptr;
	UTouUserFacingExperienceDefinition* DefaultExperience = nullptr;

	for (UObject* Object : UserExperiences)
	{
		UTouUserFacingExperienceDefinition* UserExperience = Cast<UTouUserFacingExperienceDefinition>(Object);
		if (ensure(UserExperience))
		{
			if (UserExperience->GetPrimaryAssetId() == UserExperienceId)
			{
				FoundExperience = UserExperience;
				break;
			}
			
			if (UserExperience->bIsDefaultExperience && DefaultExperience == nullptr)
			{
				DefaultExperience = UserExperience;
			}
		}
	}

	if (FoundExperience == nullptr)
	{
		FoundExperience = DefaultExperience;
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (ensure(FoundExperience && GameInstance))
	{
		// Actually host the game
		UCommonSession_HostSessionRequest* HostRequest = FoundExperience->CreateHostingRequest();
		if (ensure(HostRequest))
		{
			HostRequest->OnlineMode = OnlineMode;

			// TODO override other parameters?

			UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
			SessionSubsystem->HostSession(nullptr, HostRequest);
			
			// This will handle the map travel
		}
	}

}

void ATouGameMode::OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		// Unbind
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		UserSubsystem->OnUserInitializeComplete.RemoveDynamic(this, &ATouGameMode::OnUserInitializedForDedicatedServer);

		if (bSuccess)
		{
			// Online login worked, start a full online game
			UE_LOG(LogTouExperience, Log, TEXT("Dedicated server online login succeeded, starting online server"));
			HostDedicatedServerMatch(ECommonSessionOnlineMode::Online);
		}
		else
		{
			// Go ahead and try to host anyway, but without online support
			// This behavior is fairly game specific, but this behavior provides the most flexibility for testing
			UE_LOG(LogTouExperience, Log, TEXT("Dedicated server online login failed, starting LAN-only server"));
			HostDedicatedServerMatch(ECommonSessionOnlineMode::LAN);
		}
	}
}

void ATouGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogTouExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UTouExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UTouExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogTouExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

void ATouGameMode::OnExperienceLoaded(const UTouExperienceDefinition* CurrentExperience)
{
	// 生成任何已连接的玩家
	//@TODO: 这里我们只处理*玩家*控制器，但在 GetDefaultPawnClassForController_Implementation 中我们跳过了所有控制器。
	// 无论如何，GetDefaultPawnClassForController_Implementation 可能只针对玩家调用。
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool ATouGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UTouExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UTouExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

UClass* ATouGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UTouPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ATouGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UTouPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogTou, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogTou, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogTou, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool ATouGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

void ATouGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* ATouGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (UTouPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UTouPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ChoosePlayerStart(Player);
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATouGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (UTouPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UTouPlayerSpawningManagerComponent>())
	{
		PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	}

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool ATouGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool ATouGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	if (UTouPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UTouPlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ControllerCanRestart(Controller);
	}

	return true;
}

void ATouGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	UTouExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UTouExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnTouExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ATouGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void ATouGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

bool ATouGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

void ATouGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(LogTou, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogTou, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}
