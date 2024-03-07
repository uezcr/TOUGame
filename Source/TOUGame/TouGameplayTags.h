// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace TouGameplayTags
{
	TOUGAME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that Lyra will use
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	TOUGAME_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	TOUGAME_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	TOUGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
};
