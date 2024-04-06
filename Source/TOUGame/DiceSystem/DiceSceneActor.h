#pragma once

#include "DiceInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Interaction/InteractableTarget.h"
#include "DiceSceneActor.generated.h"


/*
 * 
 */

class ATouCharacter;

UCLASS()
class TOUGAME_API ADiceSceneActor : public AActor, public IDiceInterface, public IInteractableTarget
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADiceSceneActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TryInteract(const bool bRelease, APlayerController* PlayerController) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DiceSceneActor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Scene;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiceSceneActor", meta = (Categories = "Attribute",DisplayName = "察觉检测属性"))
	FGameplayTag PerceptionTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiceSceneActor", meta = (DisplayName = "察觉检测通过值"))
	int32 PerceptionPassValue;
	
	//测试用代码.
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInteract(const bool bRelease, APlayerController* PlayerController);
};
