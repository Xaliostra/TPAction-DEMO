// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/HUD.h"
#include "SL_HUD.generated.h"

/**
 * 
 */
class USL_MainHudWidget;
class ASL_BaseCharacter;
class UAbilitySystemComponent;
struct FCharacterLimb;

UCLASS()
class TPACTION_API ASL_HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	
	virtual void DrawHUD() override;
	void DrawWidgetsForTarget();
	void CreateMainHUD();
	void UpdateTargetASC(UAbilitySystemComponent* InASC);
	void SetupTargetWidgets(ASL_BaseCharacter* Target, const APlayerController* PC);
	void SetTargetWidgetSelected(const FGameplayTag& InTag);
	void HideTargetWidgetByTag(const FGameplayTag& InTag);
	void UnbindFromTarget(ASL_BaseCharacter* Target);
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USL_MainHudWidget> MainHUDWidgetClass;


private:
	
	UPROPERTY()
	TObjectPtr<USL_MainHudWidget> MainHUDWidget;
	
	
};
