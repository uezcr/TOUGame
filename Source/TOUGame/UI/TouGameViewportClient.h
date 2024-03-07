// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonGameViewportClient.h"
#include "TouGameViewportClient.generated.h"

class UGameInstance;
class UObject;

UCLASS()
class TOUGAME_API UTouGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	UTouGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
	
};
