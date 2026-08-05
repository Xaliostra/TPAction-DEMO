// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/SL_BlueprintLibrary.h"

#include "AbilitySystem/SL_AttributeSet.h"
#include "Characters/SL_PlayerCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"

void USL_BlueprintLibrary::SendDesiredEventToTarget(AActor* Target,
                                                              const TSubclassOf<UGameplayEffect>& DesiredEffect, const FGameplayEventData& Payload, const FGameplayTag& DataTag,
                                                              float EffectMagnitude)
{
	ASL_PlayerCharacter* PlayableCharacter = Cast<ASL_PlayerCharacter>(Target);
	if (!IsValid(PlayableCharacter)) return;
	
	USL_AttributeSet* AttributeSet = Cast<USL_AttributeSet>(PlayableCharacter->GetAttributeSet());
	if (!IsValid(AttributeSet)) return;
	
	UAbilitySystemComponent* TargetASC = PlayableCharacter->GetAbilitySystemComponent();
	if (!IsValid(TargetASC)) return;
	
	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DesiredEffect, 1.f, ContextHandle);
	
	if (DataTag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTag, EffectMagnitude);
	}
	
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
