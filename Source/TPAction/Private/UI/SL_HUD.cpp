// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SL_HUD.h"

#include "Blueprint/UserWidget.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Interfaces/Targeting/TargetableInterface.h"
#include "UI/SL_MainHudWidget.h"

void ASL_HUD::DrawHUD()
{
	Super::DrawHUD();
	if (!MainHUDWidget) return;
	
	APlayerController* PC = GetOwningPlayerController();
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(PC->GetPawn());
	if (!PlayerCharacter) return;
	
	TScriptInterface<ITargetableInterface> Target = PlayerCharacter->GetCurrentTarget();
	
	if (!Target)
	{
		//TODO: HideTargetWidgets();
		return;
	}
	
	TMap<FGameplayTag, FVector> LimbLocations = ITargetableInterface::Execute_GetTargetBonesLocations(Target.GetObject());
	
	MainHUDWidget->UpdateTargetWidgetsForLimbs(LimbLocations, PC);
}

void ASL_HUD::DrawWidgetsForTarget()
{
	
}

void ASL_HUD::CreateMainHUD()
{
	if (!MainHUDWidgetClass) return;
	if (MainHUDWidget) return;


	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		UUserWidget* MainLayout = CreateWidget<UUserWidget>(PC, MainHUDWidgetClass);
		
		if (MainLayout)
		{
			MainHUDWidget = Cast<USL_MainHudWidget>(MainLayout);
			MainHUDWidget->AddToViewport();
		}
	}
}

void ASL_HUD::UpdateTargetASC(UAbilitySystemComponent* InASC)
{
	if (!IsValid(MainHUDWidget)) return;
	
	MainHUDWidget->UpdateTargetASC(InASC);
}

void ASL_HUD::SetupTargetWidgets(ASL_BaseCharacter* Target, const APlayerController* PC)
{
	if (!IsValid(MainHUDWidget)) return;
	
	MainHUDWidget->SetupTargetWidgetsForLimbs(Target, PC);
}

void ASL_HUD::SetTargetWidgetSelected(const FGameplayTag& InTag)
{
	if (!IsValid(MainHUDWidget)) return;
	
	MainHUDWidget->SetTargetWidgetSelected(InTag);
}

void ASL_HUD::HideTargetWidgetByTag(const FGameplayTag& InTag)
{
	if (!MainHUDWidget) return;
	
	MainHUDWidget->HideTargetWidgetByTag(InTag);
}

void ASL_HUD::UnbindFromTarget(ASL_BaseCharacter* Target)
{
	if (!MainHUDWidget) return;
	
	MainHUDWidget->UnbindWidgetsFromTarget(Target);
}

