// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouSettingValueDiscrete_MobileFPSType.h"

#include "Performance/TouPerformanceSettings.h"
#include "Settings/TouSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouSettingValueDiscrete_MobileFPSType)

#define LOCTEXT_NAMESPACE "TouSettings"

UTouSettingValueDiscrete_MobileFPSType::UTouSettingValueDiscrete_MobileFPSType()
{
}

void UTouSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const UTouPlatformSpecificRenderingSettings* PlatformSettings = UTouPlatformSpecificRenderingSettings::Get();
	const UTouSettingsLocal* UserSettings = UTouSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (UTouSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 UTouSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return UTouSettingsLocal::GetDefaultMobileFrameRate();
}

FText UTouSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void UTouSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void UTouSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void UTouSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void UTouSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 UTouSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> UTouSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 UTouSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return UTouSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void UTouSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	UTouSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE

