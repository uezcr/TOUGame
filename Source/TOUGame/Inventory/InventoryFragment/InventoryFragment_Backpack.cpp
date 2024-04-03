#include "InventoryFragment_Backpack.h"

#include "Inventory/InventoryType.h"
#include "Inventory/TouInventoryItemInstance.h"

void UInventoryFragment_Backpack::OnInstanceCreated(UTouInventoryItemInstance* Instance) const
{
	if(Instance)
	{
		const FInventoryItem*InventoryItem = TableRowHandle.GetRow<FInventoryItem>(TEXT("lzy Find InventoryItem Fail"));
		check(InventoryItem);
		Instance->AddStatTagStack(InventoryItem->ItemTag,1);
	}
}
