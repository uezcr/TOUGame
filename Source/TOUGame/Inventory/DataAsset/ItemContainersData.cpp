// // Copyright Epic Games, Inc. All Rights Reserved.


#include "ItemContainersData.h"

void UItemContainersData::AddContainer(const FContainerInfo& InContainerInfo, const FIntPoint& InContainerPosition)
{
	ContainerPosition.Add(InContainerPosition);
	Containers.Add(InContainerInfo);
}

void UItemContainersData::GetContainers(TArray<FContainerInfo>& OutContainerInfo,TArray<FIntPoint>& OutContainerPosition)
{
	OutContainerInfo = Containers;
	OutContainerPosition = ContainerPosition;
}

FPrimaryAssetId UItemContainersData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FName("ItemContainer"),GetFName());
}
