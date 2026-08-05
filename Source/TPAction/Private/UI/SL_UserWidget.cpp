// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SL_UserWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/UI/SL_AttributeListener.h"
#include "Subsystems/UI/SL_UISubsystem.h"

void USL_UserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		if (USL_UISubsystem* UI = LP->GetSubsystem<USL_UISubsystem>())
		{
			UI->RegisterWidget(this);
		}
	}
}

void USL_UserWidget::InitializeAbilitySystem(UAbilitySystemComponent* InASC)
{
	if (AbilitySystem == InASC) return; 

	for (auto Listener : ActiveListeners)
	{
		if (Listener) Listener->Shutdown();
	}
	ActiveListeners.Empty();
	
	AbilitySystem = InASC;
	
	BP_OnAbilitySystemInitialized();
}

USL_AttributeListener* USL_UserWidget::ListenForAttribute(FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute, float& OutCurrentValue, float& OutMaxValue)
{
	if (!AbilitySystem) return nullptr;
	
	USL_AttributeListener* Listener = USL_AttributeListener::ListenForAttributeChange(this, AbilitySystem, Attribute, MaxAttribute);
	
	ActiveListeners.Add(Listener);
	
	bool bFound = false;
	OutCurrentValue = AbilitySystem->GetGameplayAttributeValue(Attribute, bFound);
	OutMaxValue = MaxAttribute.IsValid() ? AbilitySystem->GetGameplayAttributeValue(MaxAttribute, bFound) : 0.f;
	
	return Listener;
}

FAttributeValuePair USL_UserWidget::GetBoundAttributesValues(FGameplayAttribute Attribute,
	FGameplayAttribute MaxAttribute)
{
	if (!AbilitySystem) return {0.f, 0.f};
	
	bool bFound = false;
	
	const float CurrentValue = AbilitySystem->GetGameplayAttributeValue(Attribute, bFound);
	const float MaxValue = MaxAttribute.IsValid() ? AbilitySystem->GetGameplayAttributeValue(MaxAttribute, bFound) : 0.f;
	
	return {CurrentValue, MaxValue};
}

void USL_UserWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	for (auto Listener : ActiveListeners)
	{
		if (Listener) Listener->Shutdown();
	}
	ActiveListeners.Empty();
	
	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		if (USL_UISubsystem* UI = LP->GetSubsystem<USL_UISubsystem>())
		{
			UI->UnRegisterWidget(this);
		}
	}
}
