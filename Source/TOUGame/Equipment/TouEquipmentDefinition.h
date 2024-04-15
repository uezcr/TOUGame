#pragma once

#include "Templates/SubclassOf.h"

#include "TouEquipmentDefinition.generated.h"

class AActor;
class UTouAbilitySet;
class UTouEquipmentInstance;

USTRUCT()
struct FTouEquipmentActorToSpawn
{
	GENERATED_BODY()

	FTouEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};


/**
 * UTouEquipmentDefinition
 *
 * 可用于角色的装备的定义
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UTouEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UTouEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UTouEquipmentInstance> InstanceType;

	// 装备此装备后可获得的GA套装
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TObjectPtr<const UTouAbilitySet>> AbilitySetsToGrant;

	// 装备此装备后，将在角色身上生成的装备
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FTouEquipmentActorToSpawn> ActorsToSpawn;
};
