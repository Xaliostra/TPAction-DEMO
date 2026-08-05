// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SL_PrimaryAttackAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class TPACTION_API USL_PrimaryAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	USL_PrimaryAttackAbility();
	
protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void ApplyOnHitToSelf();
	void ApplyOnHitToTarget(const TArray<AActor*>& Targets, const TArray<FHitResult>& TargetHits);
	
protected:
	
	UPROPERTY(EditAnywhere)
	bool bAutoProgressCombo = false;
	
	UPROPERTY(EditAnywhere)
	TArray<FGameplayTagContainer> CurrentSectionActiveHand;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> EffectsApplyToSelf; 
	UPROPERTY(EditAnywhere, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> EffectsApplyToTarget; 
	
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TObjectPtr<UAnimMontage> ComboMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TArray<FName> ComboSections;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float PlayRate = 1.f;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag ComboOpenTag;
	
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag ComboCommitTag;
	
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag ComboResetTag;
	
	UPROPERTY(EditAnywhere, Category = "Combo|Tags|Input")
	FGameplayTag ComboInputTag;
	
	UPROPERTY(EditAnywhere, Category = "Tracing|Tags")
	FGameplayTag StartTracingTag;
	
	UPROPERTY(EditAnywhere, Category = "Tracing|Tags")
	FGameplayTag StopTracingTag;
	
	UPROPERTY(EditAnywhere, Category = "Tracing|Tags")
	FGameplayTag TargetHitTag;
	
	UPROPERTY(EditAnywhere, Category = "Tracing|Tags")
	FGameplayTag EventTracingTag;
	
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTag DamageTag;
	
protected:
	
	void OnComboInputReceived(const FGameplayEventData* Payload);
	
	UFUNCTION()
	void OnTracingEvent(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnTracingStartedReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnTracingStoppedReceived(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnTargetHit(FGameplayEventData Payload);
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	
	int32 CurrentComboIndex = 0;
	bool bComboWindowOpen = false;
	bool bAttackQueued = false;
	
protected:
	
	FDelegateHandle ComboOpenHandle;
	FDelegateHandle ComboCommitHandle;
	FDelegateHandle ComboResetHandle;
	
	FDelegateHandle ComboInputHandle;
	
protected:
	
	void PlayCombo();
	void JumpToCurrentSection();
	void BindGameplayEvents();
	void UnbindGameplayEvents();
	void ResetCombo();
	
protected:

	void OnComboWindowOpened(const FGameplayEventData* Payload);

	void OnComboCommit(const FGameplayEventData* Payload);
	
	void OnComboReset(const FGameplayEventData* Payload);
	
protected:
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnMontageInterrupted();
};
