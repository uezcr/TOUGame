#pragma once

#include "AbilitySystem/TouAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "TouEquipmentManagerComponent.generated.h"

class UActorComponent;
class UTouAbilitySystemComponent;
class UTouEquipmentDefinition;
class UTouEquipmentInstance;
class UTouEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FTouEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FTouAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FTouAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FTouEquipmentList;
	friend UTouEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UTouEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UTouEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FTouAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FTouEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FTouEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FTouEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FTouAppliedEquipmentEntry, FTouEquipmentList>(Entries, DeltaParms, *this);
	}

	UTouEquipmentInstance* AddEntry(TSubclassOf<UTouEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UTouEquipmentInstance* Instance);

private:
	UTouAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UTouEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FTouAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FTouEquipmentList> : public TStructOpsTypeTraitsBase2<FTouEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class UTouEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UTouEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UTouEquipmentInstance* EquipItem(TSubclassOf<UTouEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UTouEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTouEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UTouEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UTouEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UTouEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FTouEquipmentList EquipmentList;
};
