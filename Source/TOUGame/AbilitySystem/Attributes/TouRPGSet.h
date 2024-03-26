#pragma once

#include "AbilitySystemComponent.h"
#include "TouAttributeSet.h"

#include "TouRPGSet.generated.h"

class UObject;
struct FFrame;


/**
 * UTouRPGSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class UTouRPGSet : public UTouAttributeSet
{
	GENERATED_BODY()

public:

	UTouRPGSet();
	
	//Primary
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Strength);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Dexterity);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Constitution);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Intelligence);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Wisdom);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Spirit);
	//Gameplay
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Brawl);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Firearms);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Sleight);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Stealth);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, History);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Investigation);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Perception);
	ATTRIBUTE_ACCESSORS(UTouRPGSet, Medicine);

protected:

	//Primary
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Dexterity(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Constitution(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Wisdom(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Spirit(const FGameplayAttributeData& OldValue);
	
	//Gameplay
	UFUNCTION()
	void OnRep_Brawl(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Firearms(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Sleight(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Stealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_History(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Investigation(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Perception(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Medicine(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Luck(const FGameplayAttributeData& OldValue);
	
private:
	//~Begin 基础属性:游戏六种基础属性,衍生或影响其他属性,是角色的固有属性点,除升级加点外不会有其他变化.
	//力量,身体力量和肌肉力量,角色携带的物品和装备的重量上限.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Tou|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Strength;
	//敏捷,身手灵活程度,攻击精准度,撬锁和解除陷阱能力.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Dexterity, Category = "Tou|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Dexterity;
	//体质,物理层面负面状态和效果的抵抗力,最大生命值,耐力.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Constitution, Category = "Tou|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Constitution;
	//智力,解谜,研究,交流.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Tou|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Intelligence;
	//感知,周围环境、寻找隐藏物品和发现陷阱等方面的能力,敌人感知距离.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Wisdom, Category = "Tou|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Wisdom;
	//精神,对诅咒和其他超自然元素的抵抗能力,延缓SAN值降低效率,抵抗幻觉等.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Spirit, Category = "Tou|Primary", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Spirit;
	//~End 基础属性
	
	//~Begin 玩法属性:游戏过程实际用到的属性,会受到装备、道具、技能等实时影响.
	//搏斗,力量衍生,影响空手、近战武器、弓箭伤害.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Brawl, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Brawl;
	//枪械,敏捷衍生,影响枪械精度.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Firearms, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Firearms;
	//巧手,敏捷衍生,影响解除机关陷阱和开锁事件判定,摸金校尉的巧手属性高.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Sleight, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Sleight;
    //隐匿,敏捷衍生,影响潜行,跑动时惊醒怪物的概率.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stealth, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stealth;
	//历史,智力衍生,影响历史事件判断,也影响在墓中探索过程中捡起价值道具的模糊估值,摸金校尉和考古学家的历史属性都高.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_History, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData History;
	//调查,智力衍生,影响调查事件判断.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Investigation, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Investigation;
	//察觉,感知衍生,影响察觉事件判断,提前感知威胁.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Perception, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Perception;
	//医药,感知衍生,影响治疗效果,医生用感知属性是降低医生自己暴毙的概率.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Medicine, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Medicine;
	//幸运,不来自衍生,正值概率免疫伤害,负值易触发负面影响.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Luck, Category = "Tou|Gameplay", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Luck;
	//TODO:继续增加属性,将最大生命值修改成体质,是否将Health属性整合到这里.
};
