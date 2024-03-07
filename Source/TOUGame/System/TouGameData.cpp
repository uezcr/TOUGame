// Fill out your copyright notice in the Description page of Project Settings.

#include "System/TouGameData.h"
#include "TouAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouGameData)

UTouGameData::UTouGameData()
{
}

const UTouGameData& UTouGameData::Get()
{
	return UTouAssetManager::Get().GetGameData();
}
