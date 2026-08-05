// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SL_BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/Items/SL_Weapon.h"
#include "Components/SL_LimbsComponent.h"
#include "Components/SL_WeaponComponent.h"
#include "Net/UnrealNetwork.h"

void ASL_BaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bAlive);
}

ASL_BaseCharacter::ASL_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void ASL_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		ASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::OnGameplayEffectAppliedToSelf);
	}
	
	LimbsComponent = GetComponentByClass<USL_LimbsComponent>();
	if (IsValid(LimbsComponent))
	{
		LimbsComponent->OnLimbStateChanged.AddDynamic(this, &ThisClass::HandleLimbStateChanged);
	}
	
	ActiveWeaponComponent = GetComponentByClass<USL_WeaponComponent>();
}

UAbilitySystemComponent* ASL_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ASL_BaseCharacter::StartMeleeAttackTracing_Implementation(const FGameplayTagContainer& ActiveTraceTag) const
{
	USL_WeaponComponent* WeaponComponent = GetActiveWeaponComponent();
	if (!IsValid(WeaponComponent)) return;
	
	WeaponComponent->StartTrace(ActiveTraceTag);
}

void ASL_BaseCharacter::StopMeleeAttackTracing_Implementation() const
{
	IMeleeCombatInterface::StopMeleeAttackTracing_Implementation();
	
	if (!IsValid(CurrentEquippedWeapon)) return;
	
	CurrentEquippedWeapon->StopTracing();
}

ASL_Weapon* ASL_BaseCharacter::GetEquippedWeapon_Implementation() const
{
	return CurrentEquippedWeapon;
}

TArray<FCharacterLimb> ASL_BaseCharacter::GetCharacterLimbs() const
{
	if (!IsValid(LimbsComponent)) return TArray<FCharacterLimb>();
	return LimbsComponent->GetCharacterLimbs();
}

void ASL_BaseCharacter::GiveStartupAbilities()
{
	if (!IsValid(GetAbilitySystemComponent())) return;
	
	for (const auto& Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}

void ASL_BaseCharacter::InitializeAttributesWithEffect(
	const TSubclassOf<UGameplayEffect>& InitializeAttributesEffect) const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect is not set."));
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	
	for (auto Pair : InitialStatMagnitudes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Pair.Value);
	}
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ASL_BaseCharacter::InitializePrimaryAttributes() const
{
	InitializeAttributesWithEffect(PrimaryAttributesEffect);
}

void ASL_BaseCharacter::InitializeSecondaryAttributes() const
{
	InitializeAttributesWithEffect(SecondaryAttributesEffect);
}

void ASL_BaseCharacter::InitializeAdditionalGrantedEffects() const
{
	for (auto Effect : AdditionalGrantedEffects)
	{
		InitializeAttributesWithEffect(Effect);
	}
}

USL_WeaponComponent* ASL_BaseCharacter::GetActiveWeaponComponent() const
{
	if (IsValid(CurrentEquippedWeapon))
	{
		return CurrentEquippedWeapon->GetWeaponComponent();
	}
	
	return ActiveWeaponComponent;
}

void ASL_BaseCharacter::OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* InAbilitySystemComponent,
                                                      const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle) const
{
	return;
}

void ASL_BaseCharacter::HandleLimbStateChanged(FGameplayTag LimbTag, float MaxValue, float NewValue)
{
	OnLimbStateChanged.Broadcast(LimbTag, MaxValue, NewValue);
}
