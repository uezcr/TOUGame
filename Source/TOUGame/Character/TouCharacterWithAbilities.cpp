#include "TouCharacterWithAbilities.h"

#include "AbilitySystem/Attributes/TouCombatSet.h"
#include "AbilitySystem/Attributes/TouHealthSet.h"
#include "AbilitySystem/TouAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouCharacterWithAbilities)

ATouCharacterWithAbilities::ATouCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UTouAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UTouHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UTouCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void ATouCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* ATouCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

