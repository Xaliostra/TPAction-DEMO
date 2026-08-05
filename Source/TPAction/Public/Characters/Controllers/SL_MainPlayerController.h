// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Tags/GameplayTags.h"
#include "SL_MainPlayerController.generated.h"

/**
 * 
 */

class UAttributeSet;
class UAbilitySystemComponent;
class UInputMappingContext;
class UInputAction;
class ASL_HUD;
struct FInputActionValue;

UCLASS()
class TPACTION_API ASL_MainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Targeting|Flick")
	float FlickThreshold = 15.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting|Flick")
	float MinDirectionDot = 0.3f;
	
	void ProcessMouseFlick(const FVector2D& MouseDelta);
	
	FGameplayTag CurrentLimbTag = SLTags::Limbs::Center;
	bool GetLimbScreenPosition(const FGameplayTag& LimbTag, FVector2D& OutScreenPos);
	
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input") 
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Abilities")
	TObjectPtr<UInputAction> PrimaryAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Abilities")
	TObjectPtr<UInputAction> SecondaryAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Abilities")
	TObjectPtr<UInputAction> TertiaryAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input")
	TObjectPtr<UInputAction> EquipWeaponAction;
	UPROPERTY(EditDefaultsOnly, Category = "Crash|Input")
	TObjectPtr<UInputAction> ToggleTargetAction;

	UPROPERTY()
	TObjectPtr<ASL_HUD> ASL_MainHUD;
	
	void SL_Jump();
	void SL_Dodge();
	void SL_StopJumping();
	void SL_Move(const FInputActionValue& Value);
	void SL_Look(const FInputActionValue& Value);
	void SL_Primary();
	void SL_Secondary();
	void SL_Tertiary();
	void SL_EquipWeapon();
	void SL_TargetToggle();
	//void SL_ActivateAbilityByTag(const FGameplayTag& AbilityTag) const;
	
	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* InASC, UAttributeSet* InAS);
};
