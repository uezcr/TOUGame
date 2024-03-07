// // Copyright Epic Games, Inc. All Rights Reserved.

#include "TouPerformanceSettings.h"
#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/TouPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouPerformanceSettings)

UTouPlatformSpecificRenderingSettings::UTouPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const UTouPlatformSpecificRenderingSettings* UTouPlatformSpecificRenderingSettings::Get()
{
	UTouPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

UTouPerformanceSettings::UTouPerformanceSettings()
{
	PerPlatformSettings.Initialize(UTouPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FTouPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (ETouDisplayablePerformanceStat PerfStat : TEnumRange<ETouDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}
