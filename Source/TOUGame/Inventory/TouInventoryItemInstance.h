// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"

#include "TouInventoryItemInstance.generated.h"

class FLifetimeProperty;

class UTouInventoryItemDefinition;
class UTouInventoryItemFragment;
struct FFrame;
struct FGameplayTag;

/**
 * UTouInventoryItemInstance
 * 道具的实例类
 * 储存了道具的数量和TouInventoryItemDefinition
 */
UCLASS(BlueprintType)
class UTouInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UTouInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	// 向Tag添加指定数量的堆栈（如果堆栈数小于 1，则不执行任何操作）
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 从Tag中移除指定数量的堆栈（如果堆栈数小于 1，则不执行任何操作）
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// 返回指定Tag的堆栈计数（如果不存在标记，则返回 0）
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// 如果至少有一个指定Tag的堆栈，则返回 true
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	TSubclassOf<UTouInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UTouInventoryItemFragment* FindFragmentByClass(TSubclassOf<UTouInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
#if UE_WITH_IRIS
	/** 注册所有复制片段 */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	void SetItemDef(TSubclassOf<UTouInventoryItemDefinition> InDef);

	friend struct FTouInventoryList;

private:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	TSubclassOf<UTouInventoryItemDefinition> ItemDef;
};
