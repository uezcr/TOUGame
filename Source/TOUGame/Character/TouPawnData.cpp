// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouPawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouPawnData)

UTouPawnData::UTouPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}

