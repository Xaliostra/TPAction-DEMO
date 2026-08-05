// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SL_MainHudWidget.generated.h"

struct FCharacterLimb;
class USL_UserWidget;
class UOverlay;
class UAbilitySystemComponent;
class USL_LimbTargetWidget;
class ASL_BaseCharacter;
class UCanvasPanel;

UCLASS()
class TPACTION_API USL_MainHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	
	void UpdateTargetASC(UAbilitySystemComponent* InASC);
	void SetTargetWidgetSelected(const FGameplayTag& InTag);
	void SetScreenPositionForTargetWidget(UUserWidget* TargetWidget, const FVector& TargetPosition, const APlayerController* PC);
	void SetupTargetWidgetsForLimbs(ASL_BaseCharacter* Target, const APlayerController* PC);
	void UpdateTargetWidgetsForLimbs(const TMap<FGameplayTag, FVector>& LimbLocations, const APlayerController* PC);
	void HideTargetWidgetByTag(const FGameplayTag& InTag);
	void UnbindWidgetsFromTarget(ASL_BaseCharacter* Target);
	void ResetWidgets();
	
	UPROPERTY(EditAnywhere, Category = "SubWidgets")
	TSubclassOf<UUserWidget> TargetMarkWidgetClass;
	
	UPROPERTY()
	TArray<TObjectPtr<USL_LimbTargetWidget>> TargetWidgetPool;
	UPROPERTY(EditAnywhere, Category = "SubWidgets")
	int32 PoolSize = 8;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_UserWidget> TargetStatusTop_Widget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MainCanvasPanel;
};
