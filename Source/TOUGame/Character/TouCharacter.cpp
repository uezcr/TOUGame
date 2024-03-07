// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouCharacter.h"

#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "Camera/TouCameraComponent.h"
#include "Character/TouPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TouCharacterMovementComponent.h"
#include "TouGameplayTags.h"
#include "TouLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "Player/TouPlayerController.h"
#include "Player/TouPlayerState.h"
#include "System/TouSignificanceManager.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouCharacter)

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;

static FName NAME_TouCharacterCollisionProfile_Capsule(TEXT("TouPawnCapsule"));
static FName NAME_TouCharacterCollisionProfile_Mesh(TEXT("TouPawnMesh"));

ATouCharacter::ATouCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTouCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_TouCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_TouCharacterCollisionProfile_Mesh);

	UTouCharacterMovementComponent* TouMoveComp = CastChecked<UTouCharacterMovementComponent>(GetCharacterMovement());
	TouMoveComp->GravityScale = 1.0f;
	TouMoveComp->MaxAcceleration = 2400.0f;
	TouMoveComp->BrakingFrictionFactor = 1.0f;
	TouMoveComp->BrakingFriction = 6.0f;
	TouMoveComp->GroundFriction = 8.0f;
	TouMoveComp->BrakingDecelerationWalking = 1400.0f;
	TouMoveComp->bUseControllerDesiredRotation = false;
	TouMoveComp->bOrientRotationToMovement = false;
	TouMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	TouMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	TouMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	TouMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	TouMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UTouPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	CameraComponent = CreateDefaultSubobject<UTouCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void ATouCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ATouCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UTouSignificanceManager* SignificanceManager = USignificanceManager::Get<UTouSignificanceManager>(World))
		{
//@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
		}
	}
}

void ATouCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UTouSignificanceManager* SignificanceManager = USignificanceManager::Get<UTouSignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void ATouCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ATouCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
}

void ATouCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void ATouCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

ATouPlayerController* ATouCharacter::GetTouPlayerController() const
{
	return CastChecked<ATouPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ATouPlayerState* ATouCharacter::GetTouPlayerState() const
{
	return CastChecked<ATouPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UTouAbilitySystemComponent* ATouCharacter::GetTouAbilitySystemComponent() const
{
	return Cast<UTouAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ATouCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetTouAbilitySystemComponent();
}

void ATouCharacter::OnAbilitySystemInitialized()
{
	UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent();
	check(TouASC);

	InitializeGameplayTags();
}

void ATouCharacter::OnAbilitySystemUninitialized()
{
	
}

void ATouCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PawnExtComponent->HandleControllerChanged();
}

void ATouCharacter::UnPossessed()
{
	AController* const OldController = Controller;
	Super::UnPossessed();
	PawnExtComponent->HandleControllerChanged();
}

void ATouCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void ATouCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void ATouCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void ATouCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : TouGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				TouASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : TouGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				TouASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UTouCharacterMovementComponent* TouMoveComp = CastChecked<UTouCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(TouMoveComp->MovementMode, TouMoveComp->CustomMovementMode, true);
	}
}

void ATouCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		TouASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ATouCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		return TouASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ATouCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		return TouASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ATouCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		return TouASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ATouCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	
}

void ATouCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void ATouCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void ATouCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UTouCharacterMovementComponent* TouMoveComp = CastChecked<UTouCharacterMovementComponent>(GetCharacterMovement());
	TouMoveComp->StopMovementImmediately();
	TouMoveComp->DisableMovement();
}

void ATouCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void ATouCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		if (TouASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ATouCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UTouCharacterMovementComponent* TouMoveComp = CastChecked<UTouCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(TouMoveComp->MovementMode, TouMoveComp->CustomMovementMode, true);
}

void ATouCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = TouGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = TouGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			TouASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void ATouCharacter::ToggleCrouch()
{
	const UTouCharacterMovementComponent* TouMoveComp = CastChecked<UTouCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || TouMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (TouMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ATouCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		TouASC->SetLooseGameplayTagCount(TouGameplayTags::Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ATouCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UTouAbilitySystemComponent* TouASC = GetTouAbilitySystemComponent())
	{
		TouASC->SetLooseGameplayTagCount(TouGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool ATouCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void ATouCharacter::OnRep_ReplicatedAcceleration()
{
	if (UTouCharacterMovementComponent* TouMovementComponent = Cast<UTouCharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel         = TouMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians   = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		TouMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void ATouCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{

}

bool ATouCharacter::UpdateSharedReplication()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FSharedRepMovement SharedMovement;
		if (SharedMovement.FillForCharacter(this))
		{
			// Only call FastSharedReplication if data has changed since the last frame.
			// Skipping this call will cause replication to reuse the same bunch that we previously
			// produced, but not send it to clients that already received. (But a new client who has not received
			// it, will get it this frame)
			if (!SharedMovement.Equals(LastSharedReplication, this))
			{
				LastSharedReplication = SharedMovement;
				ReplicatedMovementMode = SharedMovement.RepMovementMode;

				FastSharedReplication(SharedMovement);
			}
			return true;
		}
	}

	// We cannot fastrep right now. Don't send anything.
	return false;
}

void ATouCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}

	// Timestamp is checked to reject old moves.
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Timestamp
		ReplicatedServerLastTransformUpdateTimeStamp = SharedRepMovement.RepTimeStamp;

		// Movement mode
		if (ReplicatedMovementMode != SharedRepMovement.RepMovementMode)
		{
			ReplicatedMovementMode = SharedRepMovement.RepMovementMode;
			GetCharacterMovement()->bNetworkMovementModeChanged = true;
			GetCharacterMovement()->bNetworkUpdateReceived = true;
		}

		// Location, Rotation, Velocity, etc.
		FRepMovement& MutableRepMovement = GetReplicatedMovement_Mutable();
		MutableRepMovement = SharedRepMovement.RepMovement;

		// This also sets LastRepMovement
		OnRep_ReplicatedMovement();

		// Jump force
		bProxyIsJumpForceApplied = SharedRepMovement.bProxyIsJumpForceApplied;

		// Crouch
		if (bIsCrouched != SharedRepMovement.bIsCrouched)
		{
			bIsCrouched = SharedRepMovement.bIsCrouched;
			OnRep_IsCrouched();
		}
	}
}

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;

		// Timestamp is sent as zero if unused
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			RepTimeStamp = 0.f;
		}

		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;

	// Timestamp, if non-zero.
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	}
	else
	{
		RepTimeStamp = 0.f;
	}

	return true;
}
