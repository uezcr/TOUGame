#include "DiceSceneActor.h"

#include "DiceStatics.h"



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
	BP_OnInteract(bRelease, PlayerController);
}

