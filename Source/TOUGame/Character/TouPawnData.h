// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "TouPawnData.generated.h"

class APawn;
class UTouAbilitySet;
class UTouAbilityTagRelationshipMapping;
class UTouCameraMode;
class UTouInputConfig;
class UObject;


/**
 * UTouPawnData
 *
 *	不可变的数据资产，包含用于定义棋子的属性。
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Tou Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class TOUGAME_API UTouPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UTouPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// 该棋子的实例化类（通常应派生自 ATouPawn 或 ATouCharacter）。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pawn")
	TSubclassOf<APawn> PawnClass;

	// 授予该棋子能力系统的能力套装。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Abilities")
	TArray<TObjectPtr<UTouAbilitySet>> AbilitySets;

	// 该棋子的行动应使用哪些能力标记映射
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Abilities")
	TObjectPtr<UTouAbilityTagRelationshipMapping> TagRelationshipMapping;

	// 玩家控制的棋子使用的输入配置，用于创建输入映射和绑定输入操作。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Input")
	TObjectPtr<UTouInputConfig> InputConfig;

	// 玩家控制的棋子使用的默认摄像机模式。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Camera")
	TSubclassOf<UTouCameraMode> DefaultCameraMode;
};
