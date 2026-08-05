// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SL_UISubsystem.generated.h"

/**
 * 
 */
class USL_UserWidget;
class UAbilitySystemComponent;

UCLASS()
class TPACTION_API USL_UISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	
	void RegisterWidget(USL_UserWidget* Widget);
	
	void UnRegisterWidget(USL_UserWidget* Widget);
	
	void SetAbilitySystem(UAbilitySystemComponent* InASC);
	
private:
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
	
	UPROPERTY()
	TArray<TObjectPtr<USL_UserWidget>> Widgets;
};
