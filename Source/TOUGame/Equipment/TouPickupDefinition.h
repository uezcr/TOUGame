#pragma once

#include "Engine/DataAsset.h"

#include "TouPickupDefinition.generated.h"

class UTouInventoryItemDefinition;
class UNiagaraSystem;
class UObject;
class USoundBase;
class UStaticMesh;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "Tou Pickup Data", ShortTooltip = "Data asset used to configure a pickup."))
class TOUGAME_API UTouPickupDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//TODO:lzy 枪械在用
	/*//Defines the pickup's actors to spawn, abilities to grant, and tags to add
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup|Equipment")
	TSubclassOf<UTouInventoryItemDefinition> InventoryItemDefinition;*/

	//Visual representation of the pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup|Mesh")
	TObjectPtr<UStaticMesh> DisplayMesh;

	//Cool down time between pickups in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup")
	int32 SpawnCoolDownSeconds;

	//Sound to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup")
	TObjectPtr<USoundBase> PickedUpSound;

	//Sound to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup")
	TObjectPtr<USoundBase> RespawnedSound;

	//Particle FX to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup")
	TObjectPtr<UNiagaraSystem> PickedUpEffect;

	//Particle FX to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup")
	TObjectPtr<UNiagaraSystem> RespawnedEffect;
};


UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "Tou Weapon Pickup Data", ShortTooltip = "Data asset used to configure a weapon pickup."))
class TOUGAME_API UTouWeaponPickupDefinition : public UTouPickupDefinition
{
	GENERATED_BODY()

public:

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup|Mesh")
	FVector WeaponMeshOffset;

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pickup|Mesh")
	FVector WeaponMeshScale = FVector(1.0f, 1.0f, 1.0f);
};
