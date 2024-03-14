#pragma once

#include "Logging/LogMacros.h"

class UObject;

TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTou, Log, All);
TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTouExperience, Log, All);
TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTouAbilitySystem, Log, All);
TOUGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogTouTeams, Log, All);

TOUGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
