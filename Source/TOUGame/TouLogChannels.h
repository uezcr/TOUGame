#pragma once

#include "Logging/LogMacros.h"

class UObject;

TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTou, Log, All);
TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTouExperience, Log, All);
TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTouAbilitySystem, Log, All);
TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTouTeams, Log, All);

TOUGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);

DEFINE_LOG_CATEGORY_STATIC( TOULogger, Display, All );
#define TOU_Log( FormatString, ... )                                                                                                                           \
	UE_LOG( TOULogger, Display, TEXT( "[%s line:%d,thread:%d,this:%p]: %s" ), *FString( __PRETTY_FUNCTION__ ), __LINE__, this, FPlatformTLS::GetCurrentThreadId(), \
	*FString::Printf( TEXT( FormatString ), ##__VA_ARGS__ ) )
#define TOU_Warning( FormatString, ... )                                                                                                                           \
	UE_LOG( TOULogger, Warning, TEXT( "[%s line:%d,thread:%d,this:%p]: %s" ), *FString( __PRETTY_FUNCTION__ ), __LINE__, this, FPlatformTLS::GetCurrentThreadId(), \
	*FString::Printf( TEXT( FormatString ), ##__VA_ARGS__ ) )
#define TOU_Error( FormatString, ... )                                                                                                                           \
	UE_LOG( TOULogger, Error, TEXT( "[%s line:%d,thread:%d,this:%p]: %s" ), *FString( __PRETTY_FUNCTION__ ), __LINE__, this, FPlatformTLS::GetCurrentThreadId(), \
	*FString::Printf( TEXT( FormatString ), ##__VA_ARGS__ ) )