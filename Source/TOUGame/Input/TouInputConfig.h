// // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TouInputConfig.generated.h"

class UInputAction;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FTouInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS(BlueprintType, Const)
class TOUGAME_API UTouInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UTouInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Tou|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Tou|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// 所有者使用的输入操作列表。  这些输入操作会映射到GameplayTag，必须手动绑定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTouInputAction> NativeInputActions;

	// 所有者使用的输入操作列表。  这些输入操作会映射到GameplayTag，并自动绑定到具有匹配输入Tag的能力上。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTouInputAction> AbilityInputActions;
	
};
