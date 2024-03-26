// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "TouWorldSettings.generated.h"

class UTouExperienceDefinition;

/**
 * The default world settings object, used primarily to set the default gameplay experience to use when playing on this map
 */
UCLASS()
class TOUGAME_API ATouWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	ATouWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

public:
	// 返回服务器打开此地图时使用的默认体验（如果该体验未被面向用户的体验所覆盖）。
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// 服务器打开此地图时，如果未被面向用户的体验覆盖，则使用默认体验
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UTouExperienceDefinition> DefaultGameplayExperience;

public:

#if WITH_EDITORONLY_DATA
	// 这个关卡是前端体验的一部分还是其他独立体验的一部分？
	// 设置后，当您在编辑器中点击 "播放 "时，网络模式将强制为单机模式。
	UPROPERTY(EditDefaultsOnly, Category=PIE)
	bool ForceStandaloneNetMode = false;
#endif
};
