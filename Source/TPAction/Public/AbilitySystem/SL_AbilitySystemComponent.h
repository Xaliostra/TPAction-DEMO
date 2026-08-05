// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SL_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPACTION_API USL_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	UFUNCTION(BlueprintCallable, Category = "SL|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level);
	UFUNCTION(BlueprintCallable, Category = "SL|Abilities")
	void AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);
	
private:
	
	void HandleAutoActivatedAbility(FGameplayAbilitySpec& AbilitySpec);
	
};
