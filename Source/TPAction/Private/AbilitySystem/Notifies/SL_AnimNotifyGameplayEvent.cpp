// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Notifies/SL_AnimNotifyGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"


void USL_AnimNotifyGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	
	AActor* Owner = MeshComp->GetOwner();
	
	if (!Owner) return;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
}
