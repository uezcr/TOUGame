// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouVerbMessageReplication.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/TouVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouVerbMessageReplication)

//////////////////////////////////////////////////////////////////////
// FTouVerbMessageReplicationEntry

FString FTouVerbMessageReplicationEntry::GetDebugString() const
{
	return Message.ToString();
}

//////////////////////////////////////////////////////////////////////
// FTouVerbMessageReplication

void FTouVerbMessageReplication::AddMessage(const FTouVerbMessage& Message)
{
	FTouVerbMessageReplicationEntry& NewStack = CurrentMessages.Emplace_GetRef(Message);
	MarkItemDirty(NewStack);
}

void FTouVerbMessageReplication::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// 	for (int32 Index : RemovedIndices)
	// 	{
	// 		const FGameplayTag Tag = CurrentMessages[Index].Tag;
	// 		TagToCountMap.Remove(Tag);
	// 	}
}

void FTouVerbMessageReplication::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FTouVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FTouVerbMessageReplication::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FTouVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FTouVerbMessageReplication::RebroadcastMessage(const FTouVerbMessage& Message)
{
	check(Owner);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(Owner);
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}

