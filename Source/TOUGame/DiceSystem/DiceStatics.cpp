#include "DiceStatics.h"
#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/TouRPGSet.h"
#include "Character/TouCharacter.h"

UDiceStatics::UDiceStatics()
: Super(FObjectInitializer::Get())
{
}

float UDiceStatics::GetTargetAttributeValueByTag(const FGameplayTag& GameplayTag, const ATouCharacter* Target)
{
	if(!Target) return 0.f;
	UTouAbilitySystemComponent* ASC = Target->GetTouAbilitySystemComponent();
	if(!ASC) return 0.f;
	const UTouRPGSet* AS = CastChecked<UTouRPGSet>(ASC->GetAttributeSet(UTouRPGSet::StaticClass()));
	if(!AS) return 0.f;
	return AS->GetAttributeByGamePlayTag(GameplayTag);
}

int32 UDiceStatics::RollDice20()
{
	return FMath::RandRange(1,20);
}

int32 UDiceStatics::RollDiceX(const int32 X)
{
	return FMath::RandRange(1,X);
}

int32 UDiceStatics::GetTargetModifierValueByTag(const FGameplayTag& GameplayTag, const ATouCharacter* Target)
{
	const float AttributeValue = GetTargetAttributeValueByTag(GameplayTag,Target);
	const int32 ModifierValue = FMath::Floor((AttributeValue - 10.f)/2);
	//TODO: 豁免值和其他加值.
	return FMath::Clamp(ModifierValue,-10,10);
}

void UDiceStatics::PerformCheckByTag(const FGameplayTag& GameplayTag, const ATouCharacter* Target,
	FDiceResult& DiceResult)
{
	const int32 D20 = RollDice20();
	if(D20>=20)
	{
		DiceResult.D20ResultType = ED20ResultType::Natural20;
		DiceResult.Value = 20;
	}
	else if(D20<=1)
	{
		DiceResult.D20ResultType = ED20ResultType::Natural1;
		DiceResult.Value = 1;
	}
	else
	{
		const int32 ModifierValue = GetTargetModifierValueByTag(GameplayTag,Target);
		//TODO:豁免和加值.
		DiceResult.D20ResultType = ED20ResultType::Normal;
		DiceResult.Value = FMath::Clamp(D20 + ModifierValue,1,50);
	}
}


