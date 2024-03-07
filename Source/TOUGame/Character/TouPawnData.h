// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "TouPawnData.generated.h"

class APawn;
class UTouAbilitySet;
class UTouAbilityTagRelationshipMapping;
class UTouCameraMode;
class UTouInputConfig;
class UObject;


/**
 * UTouPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Tou Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class TOUGAME_API UTouPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UTouPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ATouPawn or ATouCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Abilities")
	TArray<TObjectPtr<UTouAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Abilities")
	TObjectPtr<UTouAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Input")
	TObjectPtr<UTouInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tou|Camera")
	TSubclassOf<UTouCameraMode> DefaultCameraMode;
};
