// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouCheatManager.h"
#include "GameFramework/Pawn.h"
#include "TouPlayerController.h"
#include "TouDebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/TouAssetManager.h"
#include "System/TouGameData.h"
#include "TouGameplayTags.h"
#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/TouPawnExtensionComponent.h"
#include "System/TouSystemStatics.h"
#include "Development/TouDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouCheatManager)

DEFINE_LOG_CATEGORY(LogTouCheat);

namespace TouCheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("TouCheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("TouCheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


UTouCheatManager::UTouCheatManager()
{
	DebugCameraControllerClass = ATouDebugCameraController::StaticClass();
}

void UTouCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const FTouCheatToRun& CheatRow : GetDefault<UTouDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (TouCheat::bStartInGodMode)
	{
		God();	
	}
}

void UTouCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogTouCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void UTouCheatManager::Cheat(const FString& Msg)
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
		TouPC->ServerCheat(Msg.Left(128));
	}
}

void UTouCheatManager::CheatAll(const FString& Msg)
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
		TouPC->ServerCheatAll(Msg.Left(128));
	}
}

void UTouCheatManager::PlayNextGame()
{
	UTouSystemStatics::PlayNextGame(this);
}

void UTouCheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void UTouCheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == TouCheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool UTouCheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void UTouCheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(TouCheat::NAME_Fixed);
	}
}

void UTouCheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool UTouCheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == TouCheat::NAME_Fixed);
	}

	return false;
}

void UTouCheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void UTouCheatManager::CycleDebugCameras()
{
	if (!TouCheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void UTouCheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void UTouCheatManager::CancelActivatedAbilities()
{
	if (UTouAbilitySystemComponent* TouASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		TouASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void UTouCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = TouGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UTouAbilitySystemComponent* TouASC = GetPlayerAbilitySystemComponent())
		{
			TouASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogTouCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UTouCheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = TouGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UTouAbilitySystemComponent* TouASC = GetPlayerAbilitySystemComponent())
		{
			TouASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogTouCheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UTouCheatManager::DamageSelf(float DamageAmount)
{
	if (UTouAbilitySystemComponent* TouASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(TouASC, DamageAmount);
	}
}

void UTouCheatManager::DamageTarget(float DamageAmount)
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
		if (TouPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			TouPC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(TouPC, TargetHitResult);

		if (UTouAbilitySystemComponent* TouTargetASC = Cast<UTouAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(TouTargetASC, DamageAmount);
		}
	}
}

void UTouCheatManager::ApplySetByCallerDamage(UTouAbilitySystemComponent* TouASC, float DamageAmount)
{
	check(TouASC);

	TSubclassOf<UGameplayEffect> DamageGE = UTouAssetManager::GetSubclass(UTouGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = TouASC->MakeOutgoingSpec(DamageGE, 1.0f, TouASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(TouGameplayTags::SetByCaller_Damage, DamageAmount);
		TouASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UTouCheatManager::HealSelf(float HealAmount)
{
	if (UTouAbilitySystemComponent* TouASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(TouASC, HealAmount);
	}
}

void UTouCheatManager::HealTarget(float HealAmount)
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(TouPC, TargetHitResult);

		if (UTouAbilitySystemComponent* TouTargetASC = Cast<UTouAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(TouTargetASC, HealAmount);
		}
	}
}

void UTouCheatManager::ApplySetByCallerHeal(UTouAbilitySystemComponent* TouASC, float HealAmount)
{
	check(TouASC);

	TSubclassOf<UGameplayEffect> HealGE = UTouAssetManager::GetSubclass(UTouGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = TouASC->MakeOutgoingSpec(HealGE, 1.0f, TouASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(TouGameplayTags::SetByCaller_Heal, HealAmount);
		TouASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UTouAbilitySystemComponent* UTouCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
		return TouPC->GetTouAbilitySystemComponent();
	}
	return nullptr;
}

void UTouCheatManager::DamageSelfDestruct()
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
 		if (const UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(TouPC->GetPawn()))
		{
			if (PawnExtComp->HasReachedInitState(TouGameplayTags::InitState_GameplayReady))
			{

			}
		}
	}
}

void UTouCheatManager::God()
{
	if (ATouPlayerController* TouPC = Cast<ATouPlayerController>(GetOuterAPlayerController()))
	{
		if (TouPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			TouPC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (UTouAbilitySystemComponent* TouASC = TouPC->GetTouAbilitySystemComponent())
		{
			const FGameplayTag Tag = TouGameplayTags::Cheat_GodMode;
			const bool bHasTag = TouASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				TouASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				TouASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void UTouCheatManager::UnlimitedHealth(int32 Enabled)
{
	if (UTouAbilitySystemComponent* TouASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = TouGameplayTags::Cheat_UnlimitedHealth;
		const bool bHasTag = TouASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				TouASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				TouASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

