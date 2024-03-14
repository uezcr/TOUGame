#pragma once

#include "TouTeamInfoBase.h"

#include "TouTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class ATouTeamPrivateInfo : public ATouTeamInfoBase
{
	GENERATED_BODY()

public:
	ATouTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
