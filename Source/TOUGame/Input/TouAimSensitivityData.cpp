// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouAimSensitivityData.h"

#include "Settings/TouSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouAimSensitivityData)

UTouAimSensitivityData::UTouAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ ETouGamepadSensitivity::Slow,			0.5f },
		{ ETouGamepadSensitivity::SlowPlus,		0.75f },
		{ ETouGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ ETouGamepadSensitivity::Normal,		1.0f },
		{ ETouGamepadSensitivity::NormalPlus,	1.1f },
		{ ETouGamepadSensitivity::NormalPlusPlus,1.25f },
		{ ETouGamepadSensitivity::Fast,			1.5f },
		{ ETouGamepadSensitivity::FastPlus,		1.75f },
		{ ETouGamepadSensitivity::FastPlusPlus,	2.0f },
		{ ETouGamepadSensitivity::Insane,		2.5f },
	};
}

const float UTouAimSensitivityData::SensitivtyEnumToFloat(const ETouGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}

