#include "TouTeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "Teams/TouTeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouTeamPublicInfo)

class FLifetimeProperty;

ATouTeamPublicInfo::ATouTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ATouTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void ATouTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<UTouTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void ATouTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}

