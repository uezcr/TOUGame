#pragma once

#include "GameFramework/Actor.h"
#include "GameAICommander.generated.h"

class ATouCharacter;

UCLASS()
class TPSGAMEAIRUNTIME_API AGameAICommander : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameAICommander();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly,Category=GameAICommander)
	TArray<TObjectPtr<ATouCharacter>> PlayerList;

};
