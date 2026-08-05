// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Components/SL_LimbsComponent.h"
#include "Interfaces/Combat/MeleeCombatInterface.h"

#include "SL_BaseCharacter.generated.h"


class USL_WeaponComponent;
class USL_LimbsComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class USL_AbilitySystemComponent;
class USL_AttributeSet;
struct FCharacterLimb;

UCLASS()
class TPACTION_API ASL_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public IMeleeCombatInterface
{
	GENERATED_BODY()

public:
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	ASL_BaseCharacter();
	virtual void BeginPlay() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }
	
	virtual void StartMeleeAttackTracing_Implementation(const FGameplayTagContainer& ActiveTraceTag) const override;
	virtual void StopMeleeAttackTracing_Implementation() const override;
	virtual ASL_Weapon* GetEquippedWeapon_Implementation() const override;

	bool IsAlive() const { return bAlive; }
	void SetAlive(bool bAliveStatus) { bAlive = bAliveStatus; }
	TArray<FCharacterLimb> GetCharacterLimbs() const;
	
	FOnLimbStateChanged OnLimbStateChanged;
	
protected:
	
	void GiveStartupAbilities();
	void InitializeAttributesWithEffect(const TSubclassOf<UGameplayEffect>& InitializeAttributesEffect) const;
	void InitializePrimaryAttributes() const;
	void InitializeSecondaryAttributes() const;
	void InitializeAdditionalGrantedEffects() const;
	USL_WeaponComponent* GetActiveWeaponComponent() const;
	
	UFUNCTION()
	void OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle) const;
	
	UFUNCTION()
	void HandleLimbStateChanged(FGameplayTag LimbTag, float MaxValue, float NewValue);
	
	UPROPERTY()
	TObjectPtr<USL_LimbsComponent> LimbsComponent;
	
	UPROPERTY()
	TObjectPtr<USL_AbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<USL_AttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, Category = "Items")
	TSubclassOf<ASL_Weapon> WeaponToEquipClass; //Item Selection placeholder
	
	UPROPERTY()
	TObjectPtr<ASL_Weapon> CurrentEquippedWeapon = nullptr;
	
	UPROPERTY()
	TObjectPtr<USL_WeaponComponent> ActiveWeaponComponent;
	
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bAlive = true;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> PrimaryAttributesEffect;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> SecondaryAttributesEffect;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TArray<TSubclassOf<UGameplayEffect>> AdditionalGrantedEffects;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> ResetAttributeEffect;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TMap<FGameplayTag, float> InitialStatMagnitudes;
};
