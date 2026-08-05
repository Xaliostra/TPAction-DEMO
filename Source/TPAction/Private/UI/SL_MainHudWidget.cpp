// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SL_MainHudWidget.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Characters/SL_BaseEnemy.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SL_LimbsComponent.h"
#include "UI/SL_LimbTargetWidget.h"
#include "UI/SL_UserWidget.h"

void USL_MainHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	for (int32 i = 0; i < PoolSize; i ++)
	{
		USL_LimbTargetWidget* PooledWidget = CreateWidget<USL_LimbTargetWidget>(GetOwningPlayer(), TargetMarkWidgetClass);
		if (!PooledWidget) return;

		
		UCanvasPanelSlot* CanvasSlot = MainCanvasPanel->AddChildToCanvas(PooledWidget);
		
		if (CanvasSlot)
		{
			CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
			CanvasSlot->SetPosition(FVector2D::ZeroVector);
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			CanvasSlot->SetSize(FVector2D(32.f, 32.f));
		}
		
		TargetWidgetPool.Add(PooledWidget);

		PooledWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USL_MainHudWidget::UpdateTargetASC(UAbilitySystemComponent* InASC)
{
	if (!IsValid(InASC) || !IsValid(TargetStatusTop_Widget)) return;
	
	TargetStatusTop_Widget->InitializeAbilitySystem(InASC);
}

void USL_MainHudWidget::SetTargetWidgetSelected(const FGameplayTag& InTag)
{
	for (auto Widget : TargetWidgetPool)
	{
		if (Widget->IsHidden()) continue;
		
		if (Widget->GetLimbTag().MatchesTagExact(InTag))
		{
			Widget->SetRenderOpacity(1.0f);
		}
		else
		{
			Widget->SetRenderOpacity(0.3f);
		}
	}
}

void USL_MainHudWidget::SetScreenPositionForTargetWidget(UUserWidget* TargetWidget, const FVector& TargetPosition, const APlayerController* PC)
{
	if (!IsValid(PC) || !IsValid(TargetWidget)) return;

	FVector2D ScreenPosition;
	bool bIsOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
	   PC, 
	   TargetPosition, 
	   ScreenPosition, 
	   true 
	);

	if (bIsOnScreen)
	{
		TargetWidget->SetRenderTranslation(ScreenPosition);

		if (TargetWidget->GetVisibility() != ESlateVisibility::HitTestInvisible)
		{
			TargetWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	else
	{
		if (TargetWidget->GetVisibility() != ESlateVisibility::Collapsed)
		{
			TargetWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void USL_MainHudWidget::SetupTargetWidgetsForLimbs(ASL_BaseCharacter* Target, const APlayerController* PC)
{
	//ResetWidgets();
	
	TArray<FCharacterLimb> CharacterLimbs = Target->GetCharacterLimbs();
	TArray<FVector> LimbLocations;
	for (auto CharacterLimb : CharacterLimbs)
	{
		LimbLocations.Add(Target->GetMesh()->GetBoneLocation(CharacterLimb.DisplayBoneName));
	}
	
	if (LimbLocations.Num() != CharacterLimbs.Num()) return;
	
	int32 LimbIndex = 0;
	for (auto CharacterLimb : CharacterLimbs)
	{
		USL_LimbTargetWidget* CurrentWidget = TargetWidgetPool[LimbIndex];
		CurrentWidget->SetLimbTag(CharacterLimb.LimbTag);
		SetScreenPositionForTargetWidget(CurrentWidget, LimbLocations[LimbIndex], PC);
		CurrentWidget->BindToLimbStateChanged(Target);
		++LimbIndex;
	}
}

void USL_MainHudWidget::UpdateTargetWidgetsForLimbs(const TMap<FGameplayTag, FVector>& LimbLocations, const APlayerController* PC)
{
	for (const auto& Limb : LimbLocations)
	{
		USL_LimbTargetWidget* TargetWidget = nullptr;
		for (const auto& Widget : TargetWidgetPool)
		{
			if (Widget->GetLimbTag().MatchesTagExact(Limb.Key) && !Widget->IsHidden())
			{
				TargetWidget = Widget;
				break;
			}
		}
		SetScreenPositionForTargetWidget(TargetWidget, Limb.Value, PC);
	}
}

void USL_MainHudWidget::HideTargetWidgetByTag(const FGameplayTag& InTag)
{
	for (const auto& Widget : TargetWidgetPool)
	{
		if (Widget->GetLimbTag().MatchesTagExact(InTag))
		{
			Widget->SetRenderOpacity(0.f);
			Widget->SetHidden(true);
			return;
		}
	}
}

void USL_MainHudWidget::UnbindWidgetsFromTarget(ASL_BaseCharacter* Target)
{
	for (auto Widget : TargetWidgetPool)
	{
		if (Widget) Widget->UnbindFromTarget(Target);
	}
}

void USL_MainHudWidget::ResetWidgets()
{
	for (auto Widget : TargetWidgetPool)
	{
		if (Widget) Widget->HideWidget();
	}
}
