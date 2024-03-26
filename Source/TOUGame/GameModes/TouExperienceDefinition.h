// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "TouExperienceDefinition.generated.h"

class UGameFeatureAction;
class UTouPawnData;
class UTouExperienceActionSet;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType, Const)
class UTouExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UTouExperienceDefinition();

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// 该体验希望激活的游戏功能插件列表
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** 为玩家生成的默认棋子类别 */
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UTouPawnData> DefaultPawnData;

	// 加载/激活/停用/卸载该体验时要执行的操作列表
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// 要编入该体验的其他行动集清单
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<UTouExperienceActionSet>> ActionSets;
};
