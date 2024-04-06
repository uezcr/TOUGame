#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DiceStatics.generated.h"

UENUM(BlueprintType)
enum class ED20ResultType : uint8
{
	Normal,
	Natural20,
	Natural1
};

USTRUCT(BlueprintType)
struct FDiceResult
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly,meta = (DisplayName="结果类型"))
	ED20ResultType D20ResultType;
	UPROPERTY(BlueprintReadOnly,meta = (DisplayName="值"))
	int32 Value = 1;
};

UCLASS()
class UDiceStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UDiceStatics();
public:
	UFUNCTION(BlueprintCallable, Category = DiceStatics)
	static float GetTargetAttributeValueByTag(const FGameplayTag& GameplayTag,const ATouCharacter* Target);
	UFUNCTION(BlueprintCallable, Category = DiceStatics)
	static int32 RollDice20();
	UFUNCTION(BlueprintCallable, Category = DiceStatics)
	static int32 RollDiceX(const int32 X);
	UFUNCTION(BlueprintCallable, Category = DiceStatics)
	static int32 GetTargetModifierValueByTag(const FGameplayTag& GameplayTag,const ATouCharacter* Target);
	UFUNCTION(BlueprintCallable, Category = DiceStatics)
	static void PerformCheckByTag(const FGameplayTag& GameplayTag,const ATouCharacter* Target,FDiceResult& DiceResult);
};
