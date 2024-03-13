#pragma once

#include "AbilitySystemComponent.h"
#include "TouAttributeSet.h"

#include "TouCombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * UTouCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class UTouCombatSet : public UTouAttributeSet
{
	GENERATED_BODY()

public:

	UTouCombatSet();

	ATTRIBUTE_ACCESSORS(UTouCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UTouCombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Tou|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Tou|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
