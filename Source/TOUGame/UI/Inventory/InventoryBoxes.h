// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InventoryBoxes.generated.h"

class UUniformGridPanel;
class UGridPanel;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class TOUGAME_API UInventoryBoxes : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	//口袋,根据穿的衣服容量不一
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(BindWidget))
	UHorizontalBox*HoriBox_Pocket;

	/*//背包
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(BindWidget))
	UGridPanel*Grid_Backpack;*/

	//快捷装备
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(BindWidget))
	UUniformGridPanel*Unif_QuickEquip;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
};
