#pragma once

#include "Inventory/TouInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UTouEquipmentDefinition;
class UObject;
/*
 * TODO:目前感觉没什么用
 */
UCLASS()
class UInventoryFragment_EquippableItem : public UTouInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Tou)
	TSubclassOf<UTouEquipmentDefinition> EquipmentDefinition;
};
