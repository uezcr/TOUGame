#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "TestEnemyCharater.generated.h"

class UAbilitySystemComponent;
class UTouAbilitySystemComponent;
class UObject;

UCLASS()
class TOUGAME_API ATestEnemyCharater : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ATestEnemyCharater(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	UTouAbilitySystemComponent* GetTouAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	//~End of IGameplayTagAssetInterface
	
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
