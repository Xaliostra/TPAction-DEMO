// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SL_ANS_LockRotation.generated.h"

class AAIController;
/**
 * 
 */
UCLASS()
class TPACTION_API USL_ANS_LockRotation : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
private:
	
	UPROPERTY()
	TWeakObjectPtr<AAIController> ObservedController;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> FocusActor;
};
