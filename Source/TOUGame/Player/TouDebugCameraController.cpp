// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouDebugCameraController.h"
#include "TouCheatManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouDebugCameraController)


ATouDebugCameraController::ATouDebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as TouPlayerController to allow toggling the debug camera through cheats.
	CheatClass = UTouCheatManager::StaticClass();
}

void ATouDebugCameraController::AddCheats(bool bForce)
{
	// Mirrors TouPlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

