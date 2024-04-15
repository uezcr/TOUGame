// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/InventoryType.h"
#include "InventoryData.generated.h"

/**
 * 库存数据
 */
UCLASS(Blueprintable,BlueprintType)
class TOUGAME_API UInventoryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	//要加载的UI
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="InventoryData")
	TSubclassOf<UUserWidget> WidgetClass;

	//这个UI中的库存信息
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="InventoryData")
	TArray<FContainerInfo> Containers;

	UFUNCTION(BlueprintCallable,Category="InventoryData")
	void SetWidgetClass(TSubclassOf<UUserWidget> InWidgetClass);

	UFUNCTION(BlueprintCallable,Category="InventoryData")
	void AddContainer(const FContainerInfo& InContainer);

	UFUNCTION(BlueprintCallable,Category="InventoryData")
	void ClearData();
};
