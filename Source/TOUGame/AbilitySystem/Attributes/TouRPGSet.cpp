#include "TouRPGSet.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouRPGSet)

class FLifetimeProperty;


UTouRPGSet::UTouRPGSet()
{
}

void UTouRPGSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//TODO: 注意同步规则,暂时只将基础属性同步给自己.
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Strength, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Dexterity, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Constitution, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Intelligence, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Wisdom, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Spirit, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Brawl, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Firearms, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Sleight, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Stealth, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, History, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Investigation, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Perception, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Medicine, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTouRPGSet, Luck, COND_OwnerOnly, REPNOTIFY_Always);
	
}

void UTouRPGSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Strength, OldValue);
}

void UTouRPGSet::OnRep_Dexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Dexterity, OldValue);
}

void UTouRPGSet::OnRep_Constitution(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Constitution, OldValue);
}

void UTouRPGSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Intelligence, OldValue);
}

void UTouRPGSet::OnRep_Wisdom(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Wisdom, OldValue);
}

void UTouRPGSet::OnRep_Spirit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Spirit, OldValue);
}

void UTouRPGSet::OnRep_Brawl(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Brawl, OldValue);
}

void UTouRPGSet::OnRep_Firearms(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Firearms, OldValue);
}

void UTouRPGSet::OnRep_Sleight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Sleight, OldValue);
}

void UTouRPGSet::OnRep_Stealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Stealth, OldValue);
}

void UTouRPGSet::OnRep_History(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, History, OldValue);
}

void UTouRPGSet::OnRep_Investigation(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Investigation, OldValue);
}

void UTouRPGSet::OnRep_Perception(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Perception, OldValue);
}

void UTouRPGSet::OnRep_Medicine(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Medicine, OldValue);
}

void UTouRPGSet::OnRep_Luck(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTouRPGSet, Luck, OldValue);
}
