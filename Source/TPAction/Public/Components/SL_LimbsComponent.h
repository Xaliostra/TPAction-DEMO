// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SL_LimbsComponent.generated.h"



class UGameplayEffect;
class UAbilitySystemComponent;
struct FGameplayEffectSpec;

USTRUCT(BlueprintType)
struct FCharacterLimb
{
	GENERATED_BODY()
	
	bool IsDestroyed = false;
	
	//Supporting Limb can not be completely dissected, since it directly supports character movement; 
	//instead it gives 'Lame' modifier, that affects the movement
	UPROPERTY(EditAnywhere)
	bool bIsSupporting = {false}; 
	
	UPROPERTY(EditAnywhere)
	FGameplayTag LimbTag;
	UPROPERTY(EditDefaultsOnly)
	FName BoneName;
	UPROPERTY(EditDefaultsOnly)
	FName DisplayBoneName;
	UPROPERTY(EditDefaultsOnly)
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly)
	float Armor;
	UPROPERTY(EditDefaultsOnly)
	float LimbMultiplier;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGameplayEffect> OnLimbDestroyedEffect;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGameplayEffect> FeedbackEffect;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> LimbWound_Mesh;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> DissectedLimb_Mesh;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLimbStateChanged, FGameplayTag, LimbTag, float, MaxValue, float, CurrentValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPACTION_API USL_LimbsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USL_LimbsComponent();
	void CacheLimbBones();
	void ApplyHitToLimb(const FGameplayEffectSpec& ReceivedSpec, float FinalDamage);
	float GetLimbArmor(const FGameplayEffectSpec& ReceivedSpec);
	float GetLimbMultiplicator(const FGameplayEffectSpec& ReceivedSpec);
	FName GetLimbNameByTag(const FGameplayTag& InTag) const;
	FCharacterLimb GetLimbByTag(const FGameplayTag& InTag) const;
	TArray<FCharacterLimb> GetCharacterLimbs() const;
	bool IsBoneValidByTag(const FGameplayTag& InTag) const;
	
	FOnLimbStateChanged OnLimbStateChanged;

protected:
	
	virtual void BeginPlay() override;

private:
	
	void HandleLimbDestruction(const FGameplayTag& InTag);
	void DissectTheLimb(const FCharacterLimb& InTag);
	FCharacterLimb* GetLimbFromSpec(const FGameplayEffectSpec& ReceivedSpec);
	
	TMap<FName, FGameplayTag> LimbBonesCache;
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> InstigatorASC;
	
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTagContainer ExpectedEffectTags;
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTag ExpectedMagnitudeTag;
	
	UPROPERTY(EditAnywhere, Category = "Limbs")
	TMap<FGameplayTag, FCharacterLimb> Limbs;
	
};
