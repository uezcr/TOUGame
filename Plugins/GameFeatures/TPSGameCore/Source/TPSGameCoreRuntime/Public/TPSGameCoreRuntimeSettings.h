// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "Engine/EngineTypes.h"

#include "TPSGameCoreRuntimeSettings.generated.h"

class UObject;

/** Runtime settings specific to the TPSGameCoreRuntime plugin */
UCLASS(config = Game, defaultconfig)
class UTPSGameCoreRuntimeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTPSGameCoreRuntimeSettings(const FObjectInitializer& Initializer);

	ECollisionChannel GetAimAssistCollisionChannel() const { return AimAssistCollisionChannel; }

private:

	/**
	 * What trace channel should be used to find available targets for Aim Assist.
	 * @see UAimAssistTargetManagerComponent::GetVisibleTargets
	 */
	UPROPERTY(config, EditAnywhere, Category = "Aim Assist")
	TEnumAsByte<ECollisionChannel> AimAssistCollisionChannel = ECollisionChannel::ECC_EngineTraceChannel5;
};
