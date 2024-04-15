// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

class USizeBox;
class UBorder;
class UCanvasPanel;
class UTextBlock;
/**
 * 背包中基础插槽，背包的格子
 */
UCLASS()
class TOUGAME_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly,Category="Widget",meta=(BindWidget))
	USizeBox* SizeBox_Main;

	UPROPERTY(BlueprintReadOnly,Category="Widget",meta=(BindWidget))
	UBorder* Border_BG;

	UPROPERTY(BlueprintReadOnly,Category="Widget",meta=(BindWidget))
	UCanvasPanel* Canvas_Slot;
	
	UPROPERTY(BlueprintReadOnly,Category="Widget",meta=(BindWidget))
	UTextBlock* Text_Debug;
};
