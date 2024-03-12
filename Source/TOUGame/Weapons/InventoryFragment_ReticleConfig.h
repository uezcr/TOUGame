#pragma once

#include "Inventory/TouInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UTouReticleWidgetBase;
class UObject;

UCLASS()
class UInventoryFragment_ReticleConfig : public UTouInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<UTouReticleWidgetBase>> ReticleWidgets;
};
