// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Notifies/SL_ANS_LockRotation.h"

#include "AIController.h"
#include "GameFramework/Character.h"

void USL_ANS_LockRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	ACharacter* OwnerChar = Cast<ACharacter>(Owner);
	if (!OwnerChar) return;
	
	AController* AC = OwnerChar->GetController();
	if (!AC) return;
	
	AAIController* AIC = Cast<AAIController>(AC);
	if (!AIC) return;
	
	ObservedController = AIC;
	FocusActor = AIC->GetFocusActor();
	ObservedController->ClearFocus(EAIFocusPriority::Gameplay);
}

void USL_ANS_LockRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!ObservedController.IsValid() || !FocusActor.IsValid()) return;
	
	//ObservedController->SetFocus(FocusActor.Get(), EAIFocusPriority::Gameplay);
}
