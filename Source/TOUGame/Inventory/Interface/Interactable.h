// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

//交互类型
UENUM(Blueprintable,BlueprintType)
enum class EInteractionType:uint8
{
	//默认交互,开门，购买等
	E_Default UMETA(DisplayName="Default"),
	
	//捡东西
	E_Pickup UMETA(DisplayName="Pickup"),

	//检查道具
	E_CheckItem UMETA(DisplayName="Check Item"),

	//展示车的UI
	E_ShowCarStats UMETA(DisplayName="Show Car Stats"),

	//上车
	E_GetInCar UMETA(DisplayName="Get in Car"),

	E_Max UMETA(Hidden)
	
};

// This class does not need to be modified.
UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TOUGAME_API IInteractable
{
	GENERATED_BODY()

public:
	/* 交互的接口
	 * @param InSourceInteractable	- 那个Actor在请求交互
	 * @param InInteractionType		- 交互的类型
	 */
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Interactable")
	void OnInteract(AActor*InSourceInteractable,EInteractionType InInteractionType);

	//射线扫到物体了
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Interactable")
	void OnTraceEnter();

	//射线离开物体了
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Interactable")
	void OnTraceLeave();
};
