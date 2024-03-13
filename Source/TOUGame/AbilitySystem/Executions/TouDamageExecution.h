#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "TouDamageExecution.generated.h"

class UObject;


/**
 * UTouDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UTouDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UTouDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
