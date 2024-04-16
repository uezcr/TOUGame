// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryType.h"
#include "DataAsset/InventoryData.h"
#include "TouInventoryManagerComponent.generated.h"

class ATouCharacter;
struct FGameplayTag;
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






class UInteractable;
/**
 * 管理库存
 */
UCLASS(BlueprintType)
class TOUGAME_API UTouInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
#pragma region Parameters

	//这个库存的名字(仓库,商人,玩家)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	FString InventoryName;

	//库存信息,动态的,初始化时也会携带
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	TArray<UInventoryData*> InventoryDatas;

	//这个组件的拥有者是什么类型的
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	EInventoryType InventoryType;

	//多少秒打一次射线
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	float TraceDelta = 0.05f;

	//射线长度
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	float TraceLength = 1000.f;

	//Debug
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	bool bDebugMode=false;

	//服务器校验射线数据
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InventoryParameters")
	bool bServerVerifyTraces=false;
#pragma endregion 


public:
	UTouInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

	//~ActorComponent interface
	virtual void BeginPlay() override;
	//~End of ActorComponent interface
	
	//获取所有的实例
	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UTouInventoryItemInstance*> GetAllItems() const;
	
	
private:
	
//射线检测
#pragma region InteractionTrace
	
	UPROPERTY()
	FTimerHandle ClientTraceHandle;

	void ClientTrace();
#pragma endregion 

#pragma region RPC
#pragma endregion 

private:
	UPROPERTY(Replicated)
	FTouInventoryList InventoryList;

	//拥有这个库存组件的对象
	UPROPERTY()
	APawn*OwnerPawn;

	//扫到的对象
	UPROPERTY()
	AActor*TraceActor;
};
