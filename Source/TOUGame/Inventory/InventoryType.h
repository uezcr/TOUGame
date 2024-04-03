#pragma once
#include "GameplayTagContainer.h"
#include "InventoryType.generated.h"

//道具可以装备的类型
UENUM(Blueprintable,BlueprintType)
enum class ESlotType:uint8
{
	//可以装备到身上的 背包，衣服等
	E_Equipment UMETA(DisplayName="Equipment"),

	//武器类,可装备到1~2号快捷栏的
	E_Weapon UMETA(DisplayName="Weapon"),

	//可以装备到消耗品类3~6号快捷栏
	E_Consumable UMETA(DisplayName="Consumable"),

	//没办法装备的只能躺背包里
	E_UnableEquip UMETA(DisplayName="Unable To Equip"),
	
	E_Max,
};

//道具可以装备的类型
UENUM(Blueprintable,BlueprintType)
enum class EItemType:uint8
{
	//弹夹
	E_Magazine UMETA(DisplayName="Magazine"),

	//增益类，药品等
	E_Gain UMETA(DisplayName="Gain"),

	//只能售卖的东西,无法装备
	E_Treasures UMETA(DisplayName="Treasures"),

	//关卡里需要的道具
	E_LevelItem UMETA(DisplayName="Level Item"),
	
	E_Max,
};


USTRUCT(Blueprintable,BlueprintType)
struct FInventoryItem :public FTableRowBase
{
	GENERATED_BODY()

	//物品的唯一标识符(ID)
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag ItemTag;

	//物品可以装备到那种栏里
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	ESlotType SlotType;

	//物品可以装备到那种栏里
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	EItemType ItemType;
	
	//物品的名字
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Name;

	//物品的介绍
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Introduction;

	//当前物品在世界中的实际定义
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UTouInventoryItemDefinition> InventoryItemDefinition;
	
	//背包内的图片
	UPROPERTY(BlueprintReadOnly,Category="Backpack")
	UTexture2D* Icon;

	//物品占的行数
	UPROPERTY(BlueprintReadOnly,Category="Backpack")
	int32 RowNumber;

	//物品占的列数
	UPROPERTY(BlueprintReadOnly,Category="Backpack")
	int32 ColumnNumber;

	//物品的重量
	UPROPERTY(BlueprintReadOnly,Category="Backpack")
	int32 ItemWeight;
	
	
};
