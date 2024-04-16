#pragma once
#include "GameplayTagContainer.h"
#include "InventoryType.generated.h"

class UItemContainersData;
class UTouEquipmentDefinition;

//道具类型
UENUM(Blueprintable,BlueprintType)
enum class EItemType:uint8
{
	//其他
	E_Other UMETA(DisplayName="Other"),

	//食品
	E_Food UMETA(DisplayName="Food"),

	//武器
	E_Weapon UMETA(DisplayName="Weapon"),

	//护甲
	E_Armor UMETA(DisplayName="Armor"),
	
	//背包
	E_Backpack UMETA(DisplayName="Backpack"),

	//医疗
	E_Medical UMETA(DisplayName="Medical"),

	//汽车零件
	E_CarPart UMETA(DisplayName="Car Part"),

	//关卡里需要的道具,(打开机关的道具)
	E_LevelItem UMETA(DisplayName="Level Item"),
	
	E_Max UMETA(Hidden),
};

//交互组件的拥有者
UENUM(Blueprintable,BlueprintType)
enum class EInventoryType:uint8
{
	//玩家
	E_Player UMETA(DisplayName="Player"),

	//存储
	E_Storage UMETA(DisplayName="Storage"),

	//车
	E_Car UMETA(DisplayName="Car"),

	//商人
	E_Vendor UMETA(DisplayName="Vendor"),
	
	//AI
	E_AI UMETA(DisplayName="AI"),
	
	E_Max UMETA(Hidden),
};

//插槽的信息
USTRUCT(Blueprintable,BlueprintType)
struct FSlotInfo
{
	GENERATED_BODY()
	FSlotInfo(){}
	//插槽的类型
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo",meta = (Categories = "Inventory.Slot"))
	FGameplayTag SlotTag;

	//父级容器的ID
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 ParentContainerUID = -1;

	//当前的容器下的插槽ID
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 SlotID = 0;
	
	//道具ID
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 ItemSlotID = -1;

	//插槽现在为空吗
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	bool bIsEmpty = true;
};

//道具的信息
USTRUCT(Blueprintable,BlueprintType)
struct FItemInfos
{
	GENERATED_BODY()
	FItemInfos(){}
	//道具在表中的表行名
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ItemInfos")
	FName ItemID;

	//旋转了吗
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	bool bRotated;

	//道具数量
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 ItemAmount;
	
	//所在的插槽的ID
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 CurrentSlotID;

	//道具自己拥有的UIDs
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	TArray<int32> OwnContainerUIDs;

	//物品的装备信息,武器类的DataAsset扩展
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	UTouEquipmentDefinition*EquipData;
};

//合成时需要的物品定义
USTRUCT(Blueprintable,BlueprintType)
struct FSubItemInfo
{
	GENERATED_BODY()
	FSubItemInfo(){}
	//道具在表中的表行名
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ItemInfos")
	FName ItemID;

	//合成需要多少个
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 ItemAmount;
};

//合成信息
USTRUCT(Blueprintable,BlueprintType)
struct FCraftInfo
{
	GENERATED_BODY()
	FCraftInfo(){}
	//合成需要的原材料
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ItemInfos")
	TArray<FSubItemInfo>Ingredients;

	//合成多少个
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SlotInfo")
	int32 OutputAmount=1;
};

//库存道具
USTRUCT(Blueprintable,BlueprintType)
struct FInventoryItem :public FTableRowBase
{
	GENERATED_BODY()

	FInventoryItem(){}
	//物品的名字
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FText Name;

	//物品的介绍
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FText Introduction;

	//物品的介绍
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* ItemIcon;

	//拉伸纹理
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bStretchTexture = true;

	//拾取时的静态网格体
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh*PickupMesh;

	//物体占多少个格子
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FIntPoint ItemSize;

	//道具类型
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EItemType ItemType;

	//这个道具可以叠加吗
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bStackable;

	//最大叠加数量
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MaxStackSize=30;

	//物品的价格
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 Price;

	//物品的装备信息,武器类的DataAsset扩展
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTouEquipmentDefinition>EquipmentDefinition;

	//可以下放的插槽类型
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (Categories = "Inventory.Slot"))
	FGameplayTagContainer DroppableSlotTypes;

	/* 拥有的库存信息
	 * 拥有的库存填，如背包，防弹背心等，没有库存信息为空就行
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UItemContainersData*OwnInventoryData;

	//开始拖拽物品时的声音
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundCue*DragStartingSound;

	//结束拖拽物品时的声音
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundCue*DragEndingSound;

	//合成这个道具需要的原材料
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCraftInfo CraftingRequirements;

	//这个物品在插槽中的颜色
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FLinearColor BackgroundColor;

	//拾取完物品后这个物品销毁吗
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool DestoryOnPickup=true;
};

//容器规则
UENUM(Blueprintable,BlueprintType)
enum class EContainerRule:uint8
{
	//允许嵌套
	E_AllowNested UMETA(DisplayName="Allow Nested"),
	//允许嵌套（仅空）
	E_OnlyEmpty UMETA(DisplayName="Allow Nested (Only Empty)"),
	//不允许嵌套
	E_DontAllowNested UMETA(DisplayName="Dont Allow Nested"),
};

//容器信息
USTRUCT(Blueprintable,BlueprintType)
struct FContainerInfo
{
	GENERATED_BODY()
	FContainerInfo(){}

	//容器的ID
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	int32 ContainerUID = -1;

	//容器的插槽类型(他也是装备槽)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo",meta = (Categories = "Inventory.Slot"))
	FGameplayTag ContainerTag;

	//容器规则
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	EContainerRule NestingRule;

	//禁用物品类型(这个插槽不能装什么)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	TArray<EItemType> BannedItemType;

	//显示库存吗
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	bool bShowInventory;

	//检查空间
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	bool bCheckForSpace = true;

	//容器大小
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	FIntPoint ContainerSize;

	//容器中的插槽信息
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	TArray<FSlotInfo> Slots;

	//容器中的道具信息
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	TArray<FItemInfos> Items;

	//这个容器可以按键激活吗,没有填None
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ContainerInfo")
	FKey InputKey;
};
