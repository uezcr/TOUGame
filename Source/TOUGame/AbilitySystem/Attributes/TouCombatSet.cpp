#include "TouCombatSet.h"

#include "AbilitySystem/Attributes/TouAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouCombatSet)

class FLifetimeProperty;


UTouCombatSet::UTouCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UTouCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTouCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UTouCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouCombatSet, BaseDamage, OldValue);
}

void UTouCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouCombatSet, BaseHeal, OldValue);
}

