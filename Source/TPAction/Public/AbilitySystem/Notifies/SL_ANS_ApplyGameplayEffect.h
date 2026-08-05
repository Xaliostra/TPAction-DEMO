// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SL_ANS_ApplyGameplayEffect.generated.h"

/**
 * 
 */
class UGameplayEffect;

UCLASS()
class TPACTION_API USL_ANS_ApplyGameplayEffect : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	protected:
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> EffectClass;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag TagToAdd;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	float EffectLevel = 1.f;
};
