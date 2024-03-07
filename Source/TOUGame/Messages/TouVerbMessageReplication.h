// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "TouVerbMessage.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "TouVerbMessageReplication.generated.h"

class UObject;
struct FTouVerbMessageReplication;
struct FNetDeltaSerializeInfo;

/**
 * Represents one verb message
 */
USTRUCT(BlueprintType)
struct FTouVerbMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FTouVerbMessageReplicationEntry()
	{}

	FTouVerbMessageReplicationEntry(const FTouVerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend FTouVerbMessageReplication;

	UPROPERTY()
	FTouVerbMessage Message;
};

/** Container of verb messages to replicate */
USTRUCT(BlueprintType)
struct FTouVerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

	FTouVerbMessageReplication()
	{
	}

public:
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	// Broadcasts a message from server to clients
	void AddMessage(const FTouVerbMessage& Message);

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FTouVerbMessageReplicationEntry, FTouVerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	void RebroadcastMessage(const FTouVerbMessage& Message);

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FTouVerbMessageReplicationEntry> CurrentMessages;
	
	// Owner (for a route to a world)
	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FTouVerbMessageReplication> : public TStructOpsTypeTraitsBase2<FTouVerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
