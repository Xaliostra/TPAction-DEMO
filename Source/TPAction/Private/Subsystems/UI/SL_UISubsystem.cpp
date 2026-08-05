// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/UI/SL_UISubsystem.h"

#include "UI/SL_UserWidget.h"

void USL_UISubsystem::RegisterWidget(USL_UserWidget* Widget)
{
	if (!IsValid(Widget)) return;
	
	Widgets.AddUnique(Widget);
	
	if (AbilitySystem)
	{
		Widget->InitializeAbilitySystem(AbilitySystem);
	}
}

void USL_UISubsystem::UnRegisterWidget(USL_UserWidget* Widget)
{
	Widgets.Remove(Widget);
}

void USL_UISubsystem::SetAbilitySystem(UAbilitySystemComponent* InASC)
{
	if (AbilitySystem == InASC) return;
	
	AbilitySystem = InASC;
	
	for (USL_UserWidget* Widget : Widgets)
	{
		if (IsValid(Widget)) Widget->InitializeAbilitySystem(AbilitySystem);
	}
}
