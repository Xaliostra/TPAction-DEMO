// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "TargetableInterface.generated.h"


UINTERFACE()
class UTargetableInterface : public UInterface
{
	GENERATED_BODY()
};


class TPACTION_API ITargetableInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	bool IsTargetable() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	FVector GetTargetLocation(const FGameplayTag& InTag) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	void SetTargetMarkerVisibility(bool bVisible);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	FVector GetOwnerActorLocation() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	TMap<FGameplayTag, FVector> GetTargetBonesLocations() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	FVector GetBoneLocationByTag(const FGameplayTag& BoneTag) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting")
	bool IsBoneValidByTag(const FGameplayTag& BoneTag) const;
	
};
