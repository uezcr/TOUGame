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
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Tou_Inventory_Count, "Inventory.Count");

//////////////////////////////////////////////////////////////////////
// FTouInventoryEntry

FString FTouInventoryEntry::GetDebugString() const
{
	TSubclassOf<UTouInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (x %s)"), *GetNameSafe(Instance), *GetNameSafe(ItemDef));
}

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

UTouInventoryItemInstance* FTouInventoryList::AddEntry(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UTouInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	//看看有没有重复的,有重复的就添加到一块
	for (auto&Entry:Entries)
	{
		if(Entry.Instance->GetItemDef() == ItemDef)
		{
			Entry.Instance->AddStatTagStack(TAG_Tou_Inventory_Count,StackCount);
			return Entry.Instance;
		}
	}

	FTouInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UTouInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: 由于 UE-127172 的原因，使用行为体而不是组件作为外层
	NewEntry.Instance->SetItemDef(ItemDef);
	NewEntry.Instance->AddStatTagStack(TAG_Tou_Inventory_Count,StackCount);
	for (UTouInventoryItemFragment* Fragment : GetDefault<UTouInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	Result = NewEntry.Instance;
	
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

UTouInventoryItemInstance* UTouInventoryManagerComponent::AddItemDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UTouInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (Result && !IsReplicatedSubObjectRegistered(Result) && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
			
		}
	}
	return Result;
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

bool UTouInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	FTouInventoryEntry *RemoveEntry=nullptr;
	for (FTouInventoryEntry&Entry:InventoryList.Entries)
	{
		if (Entry.Instance->GetItemDef() != ItemDef) continue;
		RemoveEntry = &Entry;
		break;
	}

	//数量不够时移除
	if(RemoveEntry && RemoveEntry->IsValid())
	{
		InventoryList.UpdateEntryCount(RemoveEntry,-NumToConsume);
		
		//TODO:lzy 在这激活消耗类的技能
	}
	
	return true;
}

bool UTouInventoryManagerComponent::ConsumeItemsByTag(FGameplayTag InTag, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	FTouInventoryEntry* RemoveEntry=nullptr;
	for (FTouInventoryEntry&Entry:InventoryList.Entries)
	{
		if(!Entry.Instance->HasStatTag(InTag))continue;
		RemoveEntry = &Entry;

		break;
	}

	//数量不够时移除
	if(RemoveEntry && RemoveEntry->IsValid())
	{
		InventoryList.UpdateEntryCount(RemoveEntry,-NumToConsume);
		
		//TODO:lzy 在这激活消耗类的技能
	}
	
	return true;
}

int32 UTouInventoryManagerComponent::GetInventoryItemCountByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef) const
{
	if(ItemDef == nullptr) return INDEX_NONE;
	UTouInventoryItemInstance* InventoryItemInstance=FindFirstItemStackByDefinition(ItemDef);
	
	if(IsValid(InventoryItemInstance))
	{
		return InventoryItemInstance->GetStatTagStackCount(TAG_Tou_Inventory_Count);
	}
	return INDEX_NONE;
}

int32 UTouInventoryManagerComponent::GetInventoryItemCountByTag(FGameplayTag InTag)
{
	if(!InTag.IsValid()) return INDEX_NONE;

	for (auto&InventoryItemInstance:InventoryList.GetAllItems())
	{
		if (InventoryItemInstance->HasStatTag(InTag))
		{
			return InventoryItemInstance->GetStatTagStackCount(TAG_Tou_Inventory_Count);
		}
	}
	
	return INDEX_NONE;
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

bool UTouInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: 添加对堆栈限制/唯一性检查等的支持
	return true;
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


