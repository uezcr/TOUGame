// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "TouSettingsLocal.h"
#include "TouSettingsShared.h"
#include "Player/TouLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogTouGameSettingRegistry);

#define LOCTEXT_NAMESPACE "Tou"

//--------------------------------------
// UTouGameSettingRegistry
//--------------------------------------

UTouGameSettingRegistry::UTouGameSettingRegistry()
{
}

UTouGameSettingRegistry* UTouGameSettingRegistry::Get(UTouLocalPlayer* InLocalPlayer)
{
	UTouGameSettingRegistry* Registry = FindObject<UTouGameSettingRegistry>(InLocalPlayer, TEXT("TouGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UTouGameSettingRegistry>(InLocalPlayer, TEXT("TouGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool UTouGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UTouLocalPlayer* LocalPlayer = Cast<UTouLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UTouGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UTouLocalPlayer* TouLocalPlayer = Cast<UTouLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(TouLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, TouLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(TouLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(TouLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(TouLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(TouLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void UTouGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (UTouLocalPlayer* LocalPlayer = Cast<UTouLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

