// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SL_UserWidget.generated.h"

/**
 * 
 */


class UAbilitySystemComponent;
class USL_AttributeListener;

USTRUCT(BlueprintType)
struct FAttributeValuePair
{
	GENERATED_BODY()
	
	UPROPERTY()
	float CurrentValue;
	UPROPERTY()
	float MaxValue;
};

UCLASS()
class TPACTION_API USL_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void InitializeAbilitySystem(UAbilitySystemComponent* InASC);
	
protected:
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	TArray<TObjectPtr<USL_AttributeListener>> ActiveListeners;
	
	UFUNCTION(BlueprintCallable)
	USL_AttributeListener* ListenForAttribute(FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute, float& OutCurrentValue, float& OutMaxValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnAbilitySystemInitialized();
	
	UFUNCTION(BlueprintCallable)
	FAttributeValuePair GetBoundAttributesValues(FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute);
	
	virtual void NativeDestruct() override;
};
