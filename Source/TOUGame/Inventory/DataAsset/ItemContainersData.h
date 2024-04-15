// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/InventoryType.h"
#include "ItemContainersData.generated.h"

/**
 * 库存容器的数据类
 */
UCLASS(Blueprintable,BlueprintType)
class TOUGAME_API UItemContainersData : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ItemContainers")
	TArray<FIntPoint> ContainerPosition;

	//拥有的容器数据
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ItemContainers")
	TArray<FContainerInfo> Containers;

public:
	//添加容器
	UFUNCTION(BlueprintCallable,Category="ItemContainers")
	void AddContainer(const FContainerInfo&InContainerInfo,const FIntPoint& InContainerPosition);

	//获取容器
	UFUNCTION(BlueprintCallable,Category="ItemContainers")
	void GetContainers(TArray<FContainerInfo>&OutContainerInfo,TArray<FIntPoint>& OutContainerPosition);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
