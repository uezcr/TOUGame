// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "TouInventoryItemDefinition.h"
#include "TouInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Tou_Inventory_Message_StackChanged, "Tou.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// FTouInventoryEntry

FString FTouInventoryEntry::GetDebugString() const
{
	TSubclassOf<UTouInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FTouInventoryList

void FTouInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FTouInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FTouInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FTouInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FTouInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FTouInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FTouInventoryList::BroadcastChangeMessage(FTouInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FTouInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Tou_Inventory_Message_StackChanged, Message);
}

UTouInventoryItemInstance* FTouInventoryList::AddEntry(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UTouInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FTouInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UTouInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: 由于 UE-127172 的原因，使用行为体而不是组件作为外层
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UTouInventoryItemFragment* Fragment : GetDefault<UTouInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UTouInventoryItemDefinition* ItemCDO = GetDefault<UTouInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FTouInventoryList::AddEntry(UTouInventoryItemInstance* Instance)
{
	unimplemented();
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

TArray<UTouInventoryItemInstance*> FTouInventoryList::GetAllItems() const
{
	TArray<UTouInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FTouInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
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

bool UTouInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: 添加对堆栈限制/唯一性检查等的支持
	return true;
}

UTouInventoryItemInstance* UTouInventoryManagerComponent::AddItemDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UTouInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UTouInventoryManagerComponent::AddItemInstance(UTouInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UTouInventoryManagerComponent::RemoveItemInstance(UTouInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UTouInventoryItemInstance*> UTouInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UTouInventoryItemInstance* UTouInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef) const
{
	for (const FTouInventoryEntry& Entry : InventoryList.Entries)
	{
		UTouInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UTouInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FTouInventoryEntry& Entry : InventoryList.Entries)
	{
		UTouInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UTouInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N 平方，因为现在没有加速度结构
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UTouInventoryItemInstance* Instance = UTouInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
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

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class UTouInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UTouInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UTouInventoryFilter_HasTag : public UTouInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UTouInventoryItemInstance* Instance) const { return true; }
// };


