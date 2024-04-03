#pragma once

#include "Inventory/TouInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "InventoryFragment_Backpack.generated.h"

class UTouEquipmentDefinition;
class UObject;
/*
 * TODO:目前感觉没什么用
 */
UCLASS()
class UInventoryFragment_Backpack : public UTouInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Tou)
	FDataTableRowHandle TableRowHandle;
public:
	//把表里的道具Tag添加到实例里去
	virtual void OnInstanceCreated(UTouInventoryItemInstance* Instance) const override;
};
