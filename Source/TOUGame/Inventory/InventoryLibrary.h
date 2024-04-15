// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryLibrary.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class TOUGAME_API UInventoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//通过GameplayTag获取这个道具的名字
	//主要用来在DR中查找道具项
	UFUNCTION(BlueprintPure,Category="InventoryLibrary",meta = (Categories = "Inventory.Item"))
	static FName GetItemNameByGameplayTag(const FGameplayTag& InGameplayTag);
};
