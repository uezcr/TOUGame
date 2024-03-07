// // Copyright Epic Games, Inc. All Rights Reserved.


#include "AbilitySystem/TouAbilitySystemGlobals.h"
#include "TouGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouAbilitySystemGlobals)

FGameplayEffectContext* UTouAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FTouGameplayEffectContext();
}
