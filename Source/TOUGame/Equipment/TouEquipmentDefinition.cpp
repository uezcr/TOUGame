#include "TouEquipmentDefinition.h"
#include "TouEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TouEquipmentDefinition)

UTouEquipmentDefinition::UTouEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UTouEquipmentInstance::StaticClass();
}

