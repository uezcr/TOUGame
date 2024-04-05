// // Copyright Epic Games, Inc. All Rights Reserved.


#include "DiceSceneActor.h"

#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/TouRPGSet.h"
#include "Character/TouCharacter.h"


// Sets default values
ADiceSceneActor::ADiceSceneActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);
}

// Called when the game starts or when spawned
void ADiceSceneActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADiceSceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADiceSceneActor::TryInteract(const bool bRelease, APlayerController* PlayerController)
{
	BP_OnInteract(bRelease);
}

const float ADiceSceneActor::GetTargetAttributeValueByTag(const FGameplayTag& GameplayTag,
                                                          const ATouCharacter* Target) const
{
	UTouAbilitySystemComponent* ASC = Target->GetTouAbilitySystemComponent();
	if(!ASC) return 0.f;
	const UTouRPGSet* AS = CastChecked<UTouRPGSet>(ASC->GetAttributeSet(UTouRPGSet::StaticClass()));
	if(!AS) return 0.f;
	return AS->GetAttributeByGamePlayTag(GameplayTag);
}

