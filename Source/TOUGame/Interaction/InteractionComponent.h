#pragma once

#include "Components/ControllerComponent.h"
#include "InteractionComponent.generated.h"

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UInteractionComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UInteractionComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	//目标.
	UPROPERTY(BlueprintReadOnly,Category=InteractionComponent)
	TObjectPtr<AActor> TargetActor;
	//检测距离.
    UPROPERTY(EditDefaultsOnly,Category=InteractionComponent)
	float TraceRangeFromCamera = 1000.f;


private:
	//更新目标.
	void UpdateTargetActor();
	//射线检测.
	void InteractionTrace(FHitResult& HitResult) const;
	//设置新的目标.
	void SetTargetActor(const TObjectPtr<AActor> NewTargetActor);
	
};
