// // Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Inventory/InventoryBoxes.h"
#include "Input/CommonUIActionRouterBase.h"

void UInventoryBoxes::NativeOnActivated()
{
	Super::NativeOnActivated();
	UCommonUIActionRouterBase::Get(*this)->SetActiveUIInputConfig(GetDesiredInputConfig().GetValue());
}

void UInventoryBoxes::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (!GetOwningPlayer()) return;
	
	/*UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(),false);
	GetOwningPlayer()->SetShowMouseCursor(false);
	GetOwningPlayer()->ResetIgnoreLookInput();
	GetOwningPlayer()->ResetIgnoreMoveInput();*/
}
