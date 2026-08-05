// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_BaseCharacter.h"
#include "Interfaces/Targeting/TargetableInterface.h"
#include "SL_BaseEnemy.generated.h"

class UWidgetComponent;
class UGameplayEffect;


UCLASS()
class TPACTION_API ASL_BaseEnemy : public ASL_BaseCharacter, public ITargetableInterface
{
	GENERATED_BODY()

public:
	ASL_BaseEnemy();
	
	virtual bool IsTargetable_Implementation() const override { return !bIsDead; }
	virtual FVector GetTargetLocation_Implementation(const FGameplayTag& InTag) const override;
	virtual void SetTargetMarkerVisibility_Implementation(bool bVisible) override;
	virtual FVector GetOwnerActorLocation_Implementation() const override;
	virtual TMap<FGameplayTag, FVector> GetTargetBonesLocations_Implementation() const override;
	virtual FVector GetBoneLocationByTag_Implementation(const FGameplayTag& BoneTag) const override;
	virtual bool IsBoneValidByTag_Implementation(const FGameplayTag& BoneTag) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> TestAbility;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	FName TargetSocketName = TEXT("LockOn_Socket");
	
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bIsDead = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")	
	TObjectPtr<UWidgetComponent> TargetMarkerWidget;
};
