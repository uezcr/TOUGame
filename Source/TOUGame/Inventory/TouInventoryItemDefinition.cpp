// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UTouInventoryItemDefinition

UTouInventoryItemDefinition::UTouInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UTouInventoryItemFragment* UTouInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UTouInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UTouInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UTouInventoryItemDefinition

const UTouInventoryItemFragment* UTouInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UTouInventoryItemDefinition> ItemDef, TSubclassOf<UTouInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UTouInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

