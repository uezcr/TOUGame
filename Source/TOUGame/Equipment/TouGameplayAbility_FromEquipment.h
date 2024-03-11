#pragma once

#include "AbilitySystem/Abilities/TouGameplayAbility.h"

#include "TouGameplayAbility_FromEquipment.generated.h"

class UTouEquipmentInstance;
class UTouInventoryItemInstance;

/**
 * UTouGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class UTouGameplayAbility_FromEquipment : public UTouGameplayAbility
{
	GENERATED_BODY()

public:

	UTouGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Tou|Ability")
	UTouEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Tou|Ability")
	UTouInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
