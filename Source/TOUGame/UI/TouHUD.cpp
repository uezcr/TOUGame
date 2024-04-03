// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TouHUD.h"

#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryBoxes.h"

void ATouHUD::TaggleInventoryWidget()
{
	if(!InventoryBoxes)
	{
		InventoryBoxes=CreateWidget<UInventoryBoxes>(this->GetOwningPlayerController(),InventoryBoxesClass);
		InventoryBoxes->AddToViewport(2);
		InventoryBoxes->ActivateWidget();
		return;
	}

	if (InventoryBoxes->IsActivated())
	{
		InventoryBoxes->DeactivateWidget();
	}
	else
	{
		InventoryBoxes->ActivateWidget();
	}
}
