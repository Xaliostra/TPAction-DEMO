// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Notifies/SL_ANS_ApplyGameplayEffect.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void USL_ANS_ApplyGameplayEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AActor* Instigator = MeshComp->GetOwner();
	
	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Instigator))
	{
		UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent();
		
		if (ASC)
		{
			ASC->AddLooseGameplayTag(TagToAdd);
			// FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			// FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.f, ContextHandle);
			// ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void USL_ANS_ApplyGameplayEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	AActor* Instigator = MeshComp->GetOwner();
	
	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Instigator))
	{
		UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent();
		
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(TagToAdd);
		}
	}
}
