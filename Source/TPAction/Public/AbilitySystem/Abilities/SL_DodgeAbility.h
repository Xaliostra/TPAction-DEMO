// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SL_DodgeAbility.generated.h"

/**
 * 
 */
UCLASS()
class TPACTION_API USL_DodgeAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> ForwardMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> RightMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> BackwardMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> LeftMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	TArray<FGameplayTag> TagsToGrant;
	
	UPROPERTY(EditAnywhere, Category = "Motange")
	float PlayRate = 1.f;
	
	FRotator CachedControlRotation;
	
	bool bSavedUseControllerYaw;
	bool bSavedOrientRotation;
	bool bSavedDesiredRotation;
	
	bool bSavedInheritPitch;
	bool bSavedInheritYaw ;
	bool bSavedInheritRoll;
	
	
	void GrantTags();
	void ReleaseTags();
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnMontageInterrupted();
};
