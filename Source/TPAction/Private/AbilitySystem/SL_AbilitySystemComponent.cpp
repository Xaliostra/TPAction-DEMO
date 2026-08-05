// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/SL_AbilitySystemComponent.h"

#include "Tags/GameplayTags.h"


void USL_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	
	HandleAutoActivatedAbility(AbilitySpec);
}

void USL_AbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (!IsValid(GetAvatarActor()) || !GetAvatarActor()->HasAuthority()) return;
	
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level = Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void USL_AbilitySystemComponent::AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
	if (!IsValid(GetAvatarActor()) || !GetAvatarActor()->HasAuthority()) return;

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass))
	{
		AbilitySpec->Level += Level;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void USL_AbilitySystemComponent::HandleAutoActivatedAbility(FGameplayAbilitySpec& AbilitySpec)
{
	if (!IsValid(AbilitySpec.Ability)) return;
	
	if (AbilitySpec.Ability->GetAssetTags().HasTagExact(SLTags::Abilities::ActivateOnGiven))
	{
		TryActivateAbility(AbilitySpec.Handle);
	}
}
