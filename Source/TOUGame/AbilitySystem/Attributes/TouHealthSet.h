#pragma once

#include "AbilitySystemComponent.h"
#include "TouAttributeSet.h"
#include "NativeGameplayTags.h"

#include "TouHealthSet.generated.h"

class UObject;
struct FFrame;

// 声明GameplayTag
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage); //伤害.
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity); //伤害免疫.
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct); //自杀.
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld); //超出关卡边界.
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Tou_Damage_Message); //伤害消息.

struct FGameplayEffectModCallbackData;



/**
 * UTouHealthSet
 * 该类定义了受到伤害所必需的属性.
 * 属性包括:生命值,护盾和抗性等.
 */
UCLASS(BlueprintType)
class UTouHealthSet : public UTouAttributeSet
{
	GENERATED_BODY()

public:

	UTouHealthSet();

	//定义AttributeSet
	ATTRIBUTE_ACCESSORS(UTouHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UTouHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UTouHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UTouHealthSet, Damage);

	// 当生命值因受伤或治疗而改变时触发的委托，客户端可能会缺少一些信息.
	mutable FTouAttributeEvent OnHealthChanged;

	// 当最大健生命值改变时触发的委托.
	mutable FTouAttributeEvent OnMaxHealthChanged;

	// 当生命值降至零时广播的委托.
	mutable FTouAttributeEvent OnOutOfHealth;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Tou|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Tou|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category="Tou|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category="Tou|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
