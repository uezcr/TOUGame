#pragma once

#include "GameFramework/Info.h"
#include "System/GameplayTagStack.h"

#include "TouTeamInfoBase.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UTouTeamCreationComponent;
class UTouTeamSubsystem;
class UObject;
struct FFrame;

UCLASS(Abstract)
class ATouTeamInfoBase : public AInfo
{
	GENERATED_BODY()

public:
	ATouTeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	int32 GetTeamId() const { return TeamId; }

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

protected:
	virtual void RegisterWithTeamSubsystem(UTouTeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

private:
	void SetTeamId(int32 NewTeamId);

	UFUNCTION()
	void OnRep_TeamId();

public:
	friend UTouTeamCreationComponent;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamId)
	int32 TeamId;
};
