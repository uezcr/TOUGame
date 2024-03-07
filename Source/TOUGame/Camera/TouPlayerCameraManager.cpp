// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouPlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TouCameraComponent.h"
#include "TouUICameraManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouPlayerCameraManager)

class FDebugDisplayInfo;

static FName UICameraComponentName(TEXT("UICamera"));

ATouPlayerCameraManager::ATouPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = TOU_CAMERA_DEFAULT_FOV;
	ViewPitchMin = TOU_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = TOU_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UTouUICameraManagerComponent>(UICameraComponentName);
}

UTouUICameraManagerComponent* ATouPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void ATouPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ATouPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("TouPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UTouCameraComponent* CameraComponent = UTouCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}

