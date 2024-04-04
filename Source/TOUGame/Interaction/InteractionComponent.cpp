#include "Interaction/InteractionComponent.h"

#include "Physics/TouCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InteractionComponent)

namespace TouConsoleVariables
{
	static float DrawInteractionDuration = 0.0f;
	static FAutoConsoleVariableRef CVarDrawBulletTraceDuraton(
		TEXT("tou.Interaction.DrawInteractionDuration"),
		DrawInteractionDuration,
		TEXT("Should we do debug drawing for interaction traces (if above zero, sets how long (in seconds))"),
		ECVF_Default);
}

UInteractionComponent::UInteractionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTargetActor();
}

void UInteractionComponent::UpdateTargetActor()
{
	FHitResult HitResult;
	InteractionTrace(HitResult);
	if(HitResult.bBlockingHit && HitResult.GetActor())
	{
		SetTargetActor(HitResult.GetActor());
	}
	else
	{
		SetTargetActor(nullptr);
	}
}

void UInteractionComponent::InteractionTrace(FHitResult& HitResult) const
{
	FVector CamLoc;
	FRotator CamRot;
	GetPlayerViewPoint(CamLoc,CamRot);
    FVector EndLoc = CamLoc + (CamRot.Vector() * TraceRangeFromCamera);
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(InteractionTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetPawn<APawn>());
	GetWorld()->LineTraceSingleByChannel(HitResult,CamLoc,EndLoc,Tou_TraceChannel_Interaction,TraceParams);

#if ENABLE_DRAW_DEBUG
	if(TouConsoleVariables::DrawInteractionDuration > 0.f)
	{
		DrawDebugLine(GetWorld(), CamLoc, EndLoc, FColor::Cyan, false, TouConsoleVariables::DrawInteractionDuration, 0, 0.1f);
	}
#endif
}

void UInteractionComponent::SetTargetActor(TObjectPtr<AActor> NewTargetActor)
{
	if(TargetActor == NewTargetActor) return;
	TargetActor = NewTargetActor;
}
