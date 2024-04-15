// // Copyright Epic Games, Inc. All Rights Reserved.


#include "InventoryLibrary.h"

#include "GameplayTagContainer.h"

FName UInventoryLibrary::GetItemNameByGameplayTag(const FGameplayTag& InGameplayTag)
{
	FString NameString = InGameplayTag.ToString();
	int32 NameIndex =  NameString.Find(TEXT("."),ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	NameString.RemoveAt(0,NameIndex+1);
	return FName(NameString);
}
