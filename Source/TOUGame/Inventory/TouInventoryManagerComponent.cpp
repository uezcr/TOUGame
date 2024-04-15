// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "TouInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Physics/TouCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Tou_Inventory_Message_StackChanged, "Tou.Inventory.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Tou_Inventory_Count, "Inventory.Count");

//////////////////////////////////////////////////////////////////////
// FTouInventoryEntry


//////////////////////////////////////////////////////////////////////
// FTouInventoryList

void FTouInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FTouInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack,Stack.LastObservedCount);
		Stack.LastObservedCount = 0;
	}
}

void FTouInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FTouInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, Stack.LastObservedCount);
		Stack.LastObservedCount = Stack.Instance->GetStatTagStackCount(TAG_Tou_Inventory_Count);
	}
}

void FTouInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FTouInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack,Stack.LastObservedCount);
		Stack.LastObservedCount = Stack.Instance->GetStatTagStackCount(TAG_Tou_Inventory_Count);
	}
}

void FTouInventoryList::BroadcastChangeMessage(FTouInventoryEntry& Entry, int32 OldCount)
{
	FTouInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = Entry.Instance->GetStatTagStackCount(TAG_Tou_Inventory_Count);
	Message.Delta = Message.NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Tou_Inventory_Message_StackChanged, Message);
}

void FTouInventoryList::AddEntry(UTouInventoryItemInstance* Instance)
{
	unimplemented();
	//MarkItemDirty(NewEntry);
}

void FTouInventoryList::RemoveEntry(UTouInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FTouInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FTouInventoryList::UpdateEntryCount(FTouInventoryEntry* InEntry,int32 InNum)
{
	if(InNum == 0)return;
	
	bool bIsRemoveItem = false;
	
	InEntry->LastObservedCount = InEntry->Instance->GetStatTagStackCount(TAG_Tou_Inventory_Count);
	if (InNum>0)
	{
		InEntry->Instance->AddStatTagStack(TAG_Tou_Inventory_Count,InNum);
	}
	else if (InNum<0)
	{
		const int32 RemoveCount = FMath::Abs(InNum);
		bIsRemoveItem = InEntry->Instance->GetStatTagStackCount(TAG_Tou_Inventory_Count) < RemoveCount;
		InEntry->Instance->RemoveStatTagStack(TAG_Tou_Inventory_Count,RemoveCount);
	}

	if(bIsRemoveItem)
	{
		RemoveEntry(InEntry->Instance);
		return;
	}

	BroadcastChangeMessage(*InEntry,InEntry->LastObservedCount);
}

TArray<UTouInventoryItemInstance*> FTouInventoryList::GetAllItems() const
{
	TArray<UTouInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FTouInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: 希望不要在这里处理这个问题，而是将其进一步隐藏起来?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// UTouInventoryManagerComponent

UTouInventoryManagerComponent::UTouInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UTouInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

void UTouInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	
	if (GetOwnerRole()==ROLE_AutonomousProxy)
	{
		GetWorld()->GetTimerManager().SetTimer(ClientTraceHandle,this,&UTouInventoryManagerComponent::ClientTrace,TraceDelta,true);
	}
}

TArray<UTouInventoryItemInstance*> UTouInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

void UTouInventoryManagerComponent::ClientTrace()
{
	APlayerController*PC = OwnerPawn->GetLocalViewingPlayerController();
	if(!PC)return;

	FHitResult HitResult;
	FVector WorldPosition;
	FRotator WorldDirection;
	PC->GetPlayerViewPoint(WorldPosition,WorldDirection);
	FVector TraceEndPosition = WorldPosition + (WorldPosition * TraceLength);
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(InteractionTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetOwner());
	GetWorld()->LineTraceSingleByChannel(HitResult,WorldPosition,TraceEndPosition,Tou_TraceChannel_Interaction,TraceParams);
}

void UTouInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UTouInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FTouInventoryEntry& Entry : InventoryList.Entries)
		{
			UTouInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UTouInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FTouInventoryEntry& Entry : InventoryList.Entries)
	{
		UTouInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}


