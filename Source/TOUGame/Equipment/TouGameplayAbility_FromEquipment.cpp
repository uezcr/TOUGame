#include "TouGameplayAbility_FromEquipment.h"
#include "TouEquipmentInstance.h"
#include "Inventory/TouInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouGameplayAbility_FromEquipment)

UTouGameplayAbility_FromEquipment::UTouGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UTouEquipmentInstance* UTouGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UTouEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UTouInventoryItemInstance* UTouGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UTouEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UTouInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UTouGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("Tou", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
