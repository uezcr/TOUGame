// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "TouSettingValueDiscrete_PerfStat.generated.h"

enum class ETouDisplayablePerformanceStat : uint8;
enum class ETouStatDisplayMode : uint8;

class UObject;

UCLASS()
class UTouSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	UTouSettingValueDiscrete_PerfStat();

	void SetStat(ETouDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, ETouStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<ETouStatDisplayMode> DisplayModes;

	ETouDisplayablePerformanceStat StatToDisplay;
	ETouStatDisplayMode InitialMode;
};
