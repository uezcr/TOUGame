#include "TouAttributeSet.h"

#include "AbilitySystem/TouAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouAttributeSet)

class UWorld;


UTouAttributeSet::UTouAttributeSet()
{
}

UWorld* UTouAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UTouAbilitySystemComponent* UTouAttributeSet::GetTouAbilitySystemComponent() const
{
	return Cast<UTouAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

