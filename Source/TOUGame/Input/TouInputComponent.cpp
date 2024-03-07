// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/TouLocalPlayer.h"
#include "Settings/TouSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouInputComponent)

class UTouInputConfig;

UTouInputComponent::UTouInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UTouInputComponent::AddInputMappings(const UTouInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UTouInputComponent::RemoveInputMappings(const UTouInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UTouInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
