// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "TouInventoryManagerComponent.generated.h"

struct FGameplayTag;
class UTouInventoryItemDefinition;
class UTouInventoryItemInstance;
class UTouInventoryManagerComponent;
class UObject;
struct FFrame;
struct FTouInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** 物品添加到库存时的信息 */
USTRUCT(BlueprintType)
struct FTouInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UTouInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** 清单中的单个条目 */
USTRUCT(BlueprintType)
struct FTouInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FTouInventoryEntry()
	{}

	FString GetDebugString() const;

	bool IsValid()
	{
		return Instance!=nullptr;
	}

private:
	friend FTouInventoryList;
	friend UTouInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UTouInventoryItemInstance> Instance = nullptr;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FTouInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FTouInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FTouInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UTouInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FTouInventoryEntry, FTouInventoryList>(Entries, DeltaParms, *this);
	}

	UTouInventoryItemInstance* AddEntry(TSubclassOf<UTouInventoryItemDefinition> ItemClass, int32 StackCount);
	
	void AddEntry(UTouInventoryItemInstance* Instance);

	void RemoveEntry(UTouInventoryItemInstance* Instance);

	void UpdateEntryCount(FTouInventoryEntry* InEntry,int32 InNum);

private:
	void BroadcastChangeMessage(FTouInventoryEntry& Entry, int32 OldCount);

private:
	friend UTouInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FTouInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FTouInventoryList> : public TStructOpsTypeTraitsBase2<FTouInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * 管理库存
 */
UCLASS(BlueprintType)
class TOUGAME_API UTouInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTouInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//添加道具到管理器
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UTouInventoryItemInstance* AddItemDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	//获取所有的实例
	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UTouInventoryItemInstance*> GetAllItems() const;

	//查找ItemDef一样的并返回
	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UTouInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef) const;
	
	//根据ItemDef消费NumToConsume个数量的Entry
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool ConsumeItemsByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//根据GameplayTag消费NumToConsume个数量的Entry
	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category=Inventory)
	bool ConsumeItemsByTag(FGameplayTag InTag, int32 NumToConsume);

	//查找当前物品的数量
	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	int32 GetInventoryItemCountByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef) const;

	//根据GameplayTag消费NumToConsume个数量的Entry
	UFUNCTION(BlueprintCallable,Category=Inventory,BlueprintPure)
	int32 GetInventoryItemCountByTag(FGameplayTag InTag);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

//后期准备删掉的	
	//获取这个ItemDef有多少个
	int32 GetTotalItemCountByDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef) const;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UTouInventoryItemDefinition> ItemDef, int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(UTouInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(UTouInventoryItemInstance* ItemInstance);

private:
	UPROPERTY(Replicated)
	FTouInventoryList InventoryList;
};
