// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SL_LimbTargetWidget.h"

#include "Characters/SL_BaseCharacter.h"
#include "GameplayTags.h"
#include "Components/ProgressBar.h"

void USL_LimbTargetWidget::BindToLimbStateChanged(ASL_BaseCharacter* Target)
{
	Target->OnLimbStateChanged.AddDynamic(this, &ThisClass::HandleLimbStatsChanged);
}

void USL_LimbTargetWidget::UnbindFromTarget(ASL_BaseCharacter* Target)
{
	Target->OnLimbStateChanged.RemoveDynamic(this, &ThisClass::HandleLimbStatsChanged);
}

void USL_LimbTargetWidget::HideWidget()
{
	SetRenderOpacity(0.f);
	SetHidden(true);
}

void USL_LimbTargetWidget::HandleLimbStatsChanged(FGameplayTag InLimbTag, float MaxValue, float NewValue)
{
	if (!LimbTag.MatchesTagExact(InLimbTag)) return;
	
	Health_ProgressBar->SetPercent(NewValue / MaxValue);
	
	if (NewValue < 0.1f) HideWidget();
}
