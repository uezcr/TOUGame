// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TouHUD.generated.h"

class UInventoryBoxes;
/**
 * 
 */
UCLASS()
class TOUGAME_API ATouHUD : public AHUD
{
	GENERATED_BODY()
public:
	//开关主交互框
	UFUNCTION(BlueprintCallable,Category="TaggleWidget")
	void TaggleInventoryWidget();

	UFUNCTION(BlueprintPure,Category="TaggleWidget")
	FORCEINLINE UInventoryBoxes* GetInventoryBoxes(){return InventoryBoxes;}

protected:
	UPROPERTY(EditDefaultsOnly,Category="Widget|Class",meta=(AllowedClasses="InventoryBoxes"))
	TSubclassOf<UUserWidget>InventoryBoxesClass;
		
private:
	//主交互框
	UPROPERTY()
	UInventoryBoxes* InventoryBoxes;
};
