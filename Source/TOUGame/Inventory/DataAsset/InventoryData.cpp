// // Copyright Epic Games, Inc. All Rights Reserved.


#include "InventoryData.h"

FPrimaryAssetId UInventoryData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FName("InventoryData"),GetFName());
}

void UInventoryData::SetWidgetClass(TSubclassOf<UUserWidget> InWidgetClass)
{
	WidgetClass = InWidgetClass;
}

void UInventoryData::AddContainer(const FContainerInfo& InContainer)
{
	Containers.Add(InContainer);
}

void UInventoryData::ClearData()
{
	WidgetClass=nullptr;
	Containers.Empty();
}
