#pragma once

#include "Character/TouCharacter.h"

#include "TouCharacterWithAbilities.generated.h"

class UAbilitySystemComponent;
class UTouAbilitySystemComponent;
class UObject;

// ATouCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
UCLASS(Blueprintable)
class TOUGAME_API ATouCharacterWithAbilities : public ATouCharacter
{
	GENERATED_BODY()

public:
	ATouCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Tou|PlayerState")
	TObjectPtr<UTouAbilitySystemComponent> AbilitySystemComponent;
	
	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UTouHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UTouCombatSet> CombatSet;
};
