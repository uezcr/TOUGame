// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TouVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FTouVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class TOUGAME_API UTouVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Tou")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Tou")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Tou")
	static FGameplayCueParameters VerbMessageToCueParameters(const FTouVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Tou")
	static FTouVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
