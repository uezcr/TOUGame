// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TouInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UTouInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class TOUGAME_API UTouInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UTouInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UTouInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class UTouInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UTouInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UTouInventoryItemFragment>> Fragments;

public:
	const UTouInventoryItemFragment* FindFragmentByClass(TSubclassOf<UTouInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UTouInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UTouInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UTouInventoryItemDefinition> ItemDef, TSubclassOf<UTouInventoryItemFragment> FragmentClass);
};