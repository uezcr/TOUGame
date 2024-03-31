// // Copyright Epic Games, Inc. All Rights Reserved.


#include "DiceSceneActor.h"

#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/TouRPGSet.h"
#include "Character/TouCharacter.h"


// Sets default values
ADiceSceneActor::ADiceSceneActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

const float ADiceSceneActor::GetTargetAttributeValueByTag(const FGameplayTag& GameplayTag,
	const ATouCharacter* Target) const
{
	UTouAbilitySystemComponent* ASC = Target->GetTouAbilitySystemComponent();
	if(!ASC) return 0.f;
	const UTouRPGSet* AS = CastChecked<UTouRPGSet>(ASC->GetAttributeSet(UTouRPGSet::StaticClass()));
	if(!AS) return 0.f;
	return AS->GetAttributeByGamePlayTag(GameplayTag);
}

