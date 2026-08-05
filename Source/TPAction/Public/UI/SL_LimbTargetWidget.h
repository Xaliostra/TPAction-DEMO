// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SL_UserWidget.h"
#include "SL_LimbTargetWidget.generated.h"

class UProgressBar;
class ASL_BaseCharacter;

UCLASS()
class TPACTION_API USL_LimbTargetWidget : public USL_UserWidget
{
	GENERATED_BODY()
	
public:
	
	void SetLimbTag(const FGameplayTag& InTag) {LimbTag = InTag;}
	FGameplayTag GetLimbTag() const {return LimbTag;}
	void BindToLimbStateChanged(ASL_BaseCharacter* Target);
	void UnbindFromTarget(ASL_BaseCharacter* Target);
	bool IsHidden() const {return bIsHidden;}
	void SetHidden(bool bHidden) {bIsHidden = bHidden;}
	void HideWidget();
	
private:
	
	bool bIsHidden = false;
	
	FDelegateHandle ObservedTargetHandle;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressBar;
	
	UPROPERTY()
	FGameplayTag LimbTag; 
	
	UFUNCTION()
	void HandleLimbStatsChanged(FGameplayTag InLimbTag, float MaxValue, float NewValue);
};
