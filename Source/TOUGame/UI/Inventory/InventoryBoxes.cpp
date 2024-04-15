// // Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Inventory/InventoryBoxes.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/CommonUIActionRouterBase.h"

void UInventoryBoxes::NativeOnActivated()
{
	Super::NativeOnActivated();
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(),this);
	GetOwningPlayer()->SetShowMouseCursor(true);
}

void UInventoryBoxes::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (!GetOwningPlayer()) return;
	
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(),false);
	GetOwningPlayer()->SetShowMouseCursor(false);
}
