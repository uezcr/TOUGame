#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "TouHealExecution.generated.h"

class UObject;


/**
 * UTouHealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class UTouHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UTouHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
