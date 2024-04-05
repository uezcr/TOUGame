#pragma once

#include "UObject/Interface.h"
#include "InteractableTarget.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TOUGAME_API IInteractableTarget
{
	GENERATED_BODY()

public:
	virtual void TryInteract(const bool bRelease,APlayerController* PlayerController) = 0;
};
