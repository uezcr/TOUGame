// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"
#include "LoadingProcessInterface.h"

#include "TouExperienceManagerComponent.generated.h"

namespace UE::GameFeatures { struct FResult; }

class UTouExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTouExperienceLoaded, const UTouExperienceDefinition* /*Experience*/);

enum class ETouExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS()
class UTouExperienceManagerComponent final : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:

	UTouExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	// 尝试设置当前体验，无论是用户界面还是游戏体验
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// 确保在加载体验后调用委托、
	// 在调用其他委托之前。
	// 但是，如果体验已加载，则会立即调用委托。
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnTouExperienceLoaded::FDelegate&& Delegate);

	// 确保在加载体验后调用委托
	// 如果体验已加载，则立即调用委托
	void CallOrRegister_OnExperienceLoaded(FOnTouExperienceLoaded::FDelegate&& Delegate);

	// 确保在加载体验后调用委托
	// 如果体验已加载，则立即调用委托
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnTouExperienceLoaded::FDelegate&& Delegate);

	// 如果当前体验已完全加载，则返回当前体验，否则将断言
	// （即，如果您过早调用它）
	const UTouExperienceDefinition* GetCurrentExperienceChecked() const;

	// 如果体验已完全加载，则返回 true
	bool IsExperienceLoaded() const;

private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();

private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UTouExperienceDefinition> CurrentExperience;

	ETouExperienceLoadState LoadState = ETouExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/**
	 * Delegate called when the experience has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 */
	FOnTouExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnTouExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnTouExperienceLoaded OnExperienceLoaded_LowPriority;
};
