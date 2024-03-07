// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/TouLocalPlayer.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Settings/TouSettingsLocal.h"
#include "Settings/TouSettingsShared.h"
#include "CommonUserSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouLocalPlayer)

class UObject;

UTouLocalPlayer::UTouLocalPlayer()
{
}

void UTouLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UTouSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UTouLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void UTouLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);
}

bool UTouLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	return bResult;
}

void UTouLocalPlayer::InitOnlineSession()
{
	Super::InitOnlineSession();
}

UTouSettingsLocal* UTouLocalPlayer::GetLocalSettings() const
{
	return UTouSettingsLocal::Get();
}

UTouSettingsShared* UTouLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		// On PC it's okay to use the sync load because it only checks the disk
		// This could use a platform tag to check for proper save support instead
		bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;
		
		if (bCanLoadBeforeLogin)
		{
			SharedSettings = UTouSettingsShared::LoadOrCreateSettings(this);
		}
		else
		{
			// We need to wait for user login to get the real settings so return temp ones
			SharedSettings = UTouSettingsShared::CreateTemporarySettings(this);
		}
	}

	return SharedSettings;
}

void UTouLocalPlayer::LoadSharedSettingsFromDisk(bool bForceLoad)
{
	FUniqueNetIdRepl CurrentNetId = GetCachedUniqueNetId();
	if (!bForceLoad && SharedSettings && CurrentNetId == NetIdForSharedSettings)
	{
		// Already loaded once, don't reload
		return;
	}

	ensure(UTouSettingsShared::AsyncLoadOrCreateSettings(this, UTouSettingsShared::FOnSettingsLoadedEvent::CreateUObject(this, &UTouLocalPlayer::OnSharedSettingsLoaded)));
}

void UTouLocalPlayer::OnSharedSettingsLoaded(UTouSettingsShared* LoadedOrCreatedSettings)
{
	// The settings are applied before it gets here
	if (ensure(LoadedOrCreatedSettings))
	{
		// This will replace the temporary or previously loaded object which will GC out normally
		SharedSettings = LoadedOrCreatedSettings;

		NetIdForSharedSettings = GetCachedUniqueNetId();
	}
}

void UTouLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UTouLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}


