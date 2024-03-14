#pragma once

#include "TouTeamInfoBase.h"

#include "TouTeamPublicInfo.generated.h"

class UTouTeamCreationComponent;
class UTouTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class ATouTeamPublicInfo : public ATouTeamInfoBase
{
	GENERATED_BODY()

	friend UTouTeamCreationComponent;

public:
	ATouTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UTouTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UTouTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<UTouTeamDisplayAsset> TeamDisplayAsset;
};
