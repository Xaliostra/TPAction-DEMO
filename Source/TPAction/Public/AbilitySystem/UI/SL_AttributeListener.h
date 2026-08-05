// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SL_AttributeListener.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeChangedChangedDelegate, float, NewValue, float, OldValue, float, MaxValue);

UCLASS()
class TPACTION_API USL_AttributeListener : public UObject
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static USL_AttributeListener* ListenForAttributeChange(UObject* WorldContextObject, UAbilitySystemComponent* ASC, FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute);

	UPROPERTY(BlueprintAssignable)
	FAttributeChangedChangedDelegate OnAttributeChanged;
	
	UFUNCTION(BlueprintCallable)
	void Shutdown();
	
private:
	
	void OnAttributeChangedCallback(const FOnAttributeChangeData& Data);
	void OnMaxAttributeChangedCallback(const FOnAttributeChangeData& Data);
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC;
	
	FGameplayAttribute CurrentAttribute;
	FGameplayAttribute CurrentMaxAttribute;
	
	FDelegateHandle AttributeHandle;
	FDelegateHandle MaxAttributeHandle;
};
