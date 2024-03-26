#pragma once

#include "Inventory/TouInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UTouEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UTouInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Tou)
	TSubclassOf<UTouEquipmentDefinition> EquipmentDefinition;
};
