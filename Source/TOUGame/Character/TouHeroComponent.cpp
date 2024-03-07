// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "TouLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "Player/TouPlayerController.h"
#include "Player/TouPlayerState.h"
#include "Player/TouLocalPlayer.h"
#include "Character/TouPawnExtensionComponent.h"
#include "Character/TouPawnData.h"
#include "Character/TouCharacter.h"
#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "Input/TouInputConfig.h"
#include "Input/TouInputComponent.h"
#include "Camera/TouCameraComponent.h"
#include "TouGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "Camera/TouCameraMode.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace TouHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UTouHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UTouHeroComponent::NAME_ActorFeatureName("Hero");

UTouHeroComponent::UTouHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UTouHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogTou, Error, TEXT("[UTouHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("TouHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("TouHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UTouHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == TouGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == TouGameplayTags::InitState_Spawned && DesiredState == TouGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ATouPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ATouPlayerController* TouPC = GetController<ATouPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !TouPC || !TouPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == TouGameplayTags::InitState_DataAvailable && DesiredState == TouGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ATouPlayerState* TouPS = GetPlayerState<ATouPlayerState>();

		return TouPS && Manager->HasFeatureReachedInitState(Pawn, UTouPawnExtensionComponent::NAME_ActorFeatureName, TouGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == TouGameplayTags::InitState_DataInitialized && DesiredState == TouGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UTouHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == TouGameplayTags::InitState_DataAvailable && DesiredState == TouGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ATouPlayerState* TouPS = GetPlayerState<ATouPlayerState>();
		if (!ensure(Pawn && TouPS))
		{
			return;
		}

		const UTouPawnData* PawnData = nullptr;

		if (UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UTouPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(TouPS->GetTouAbilitySystemComponent(), TouPS);
		}

		if (ATouPlayerController* TouPC = GetController<ATouPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		if (PawnData)
		{
			if (UTouCameraComponent* CameraComponent = UTouCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void UTouHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UTouPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == TouGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UTouHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { TouGameplayTags::InitState_Spawned, TouGameplayTags::InitState_DataAvailable, TouGameplayTags::InitState_DataInitialized, TouGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UTouHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UTouPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(TouGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UTouHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UTouHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UTouLocalPlayer* LP = Cast<UTouLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UTouPawnData* PawnData = PawnExtComp->GetPawnData<UTouPawnData>())
		{
			if (const UTouInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}
							
							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The Tou Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UTouInputComponent or modify this component accordingly.
				UTouInputComponent* TouIC = Cast<UTouInputComponent>(PlayerInputComponent);
				if (ensureMsgf(TouIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UTouInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					TouIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					TouIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					TouIC->BindNativeAction(InputConfig, TouGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					TouIC->BindNativeAction(InputConfig, TouGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					TouIC->BindNativeAction(InputConfig, TouGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
					TouIC->BindNativeAction(InputConfig, TouGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UTouHeroComponent::AddAdditionalInputConfig(const UTouInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UTouInputComponent* TouIC = Pawn->FindComponentByClass<UTouInputComponent>();
		if (ensureMsgf(TouIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UTouInputComponent or a subclass of it.")))
		{
			TouIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UTouHeroComponent::RemoveAdditionalInputConfig(const UTouInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UTouHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UTouHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UTouAbilitySystemComponent* TouASC = PawnExtComp->GetTouAbilitySystemComponent())
			{
				TouASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UTouHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UTouAbilitySystemComponent* TouASC = PawnExtComp->GetTouAbilitySystemComponent())
		{
			TouASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UTouHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UTouHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UTouHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * TouHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * TouHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void UTouHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (ATouCharacter* Character = GetPawn<ATouCharacter>())
	{
		Character->ToggleCrouch();
	}
}

TSubclassOf<UTouCameraMode> UTouHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UTouPawnExtensionComponent* PawnExtComp = UTouPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UTouPawnData* PawnData = PawnExtComp->GetPawnData<UTouPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UTouHeroComponent::SetAbilityCameraMode(TSubclassOf<UTouCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UTouHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

