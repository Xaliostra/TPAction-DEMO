// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SL_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "Tags/GameplayTags.h"
#include "SL_PlayerCharacter.generated.h"

class ITargetableInterface;
class USL_AttributeSet;
class USL_AbilitySystemComponent;
class ASL_BaseEnemy;
class ASL_Weapon;
class ASL_HUD;
class USpringArmComponent;
class UCameraComponent;
struct FCharacterLimb;


USTRUCT(BlueprintType)
struct FLocomotionSelection
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 AnimationIndex = 0;
	UPROPERTY(BlueprintReadOnly)
	float WarpAngle = 0;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnASCInitialized, UAbilitySystemComponent* ASC, UAttributeSet* AS);

UCLASS()
class TPACTION_API ASL_PlayerCharacter : public ASL_BaseCharacter
{
	GENERATED_BODY()

public:
	
	ASL_PlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	void BlockTargeting() { bIsTargetingBlocked = true; }
	void UnlockTargeting() { bIsTargetingBlocked = false; }
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsLockedOn() const { return bIsLockedOn; }
	
	void BindEventsToASC();
	
	UFUNCTION(BlueprintPure)
	FLocomotionSelection GetLocomotionSelection() const;
	
	void SpawnAndEquipWeapon();
	void OnPrimaryAttack();
	void OnDodge();
	void FindTarget();
	TScriptInterface<ITargetableInterface> GetCurrentTarget() const;
	void SetNewLimbTarget(const FGameplayTag& NewLimbTag);
	
	FOnASCInitialized OnASCInitialized;
	bool bIsAbilitySystemInitialized = false;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Juice")
	float DefaultFOV = 90.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Juice")
	float DashFOV = 105.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera|Juice")
	float FOVInterpSpeed = 8.0f;
	
private:
	
	FGameplayTag SelectedEnemyLimbTag = SLTags::Limbs::Center;
	

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRadius = 3500.f;
	
	UPROPERTY()
	TScriptInterface<ITargetableInterface> CurrentTarget;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float MinPitchDown = -20.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float MaxPitchUp = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float BaseCameraHeight = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float MaxCameraHeight = 120.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float LockOnRotationLagSpeed = 6.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float DefaultRotationLagSpeed = 12.0f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")
	float BaseTargetArmLength = 300.f;
	
	bool bIsTargetingBlocked {false};
	bool bIsLockedOn {false};
	
	UPROPERTY()
	TObjectPtr<ASL_HUD> ASLHUD;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag IsDodgingTag;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	FGameplayTag PrimaryAbilityTag;
	UPROPERTY(EditAnywhere, Category = "Abilities|Input")
	FGameplayTag PrimaryInputTag;
	UPROPERTY(EditAnywhere, Category = "Abilities|Movement")
	FGameplayTag MovementLockedTag;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	FGameplayTag DodgeAbilityTag;
	
	UPROPERTY(EditAnywhere, Category = "Items")
	FName RightWeaponSocketName;
	
	void UpdateCamera_Locked(float DeltaSeconds);
	void SetTargetLockActive(bool bIsLocked);
	void ClearTarget();
	void InterpolateCamera(const float DeltaTime);
	bool IsActorValidTarget(AActor* InActor) const;
	
	UFUNCTION()
	void ToggleMovement(const FGameplayTag Tag, int32 NewCount);
};
