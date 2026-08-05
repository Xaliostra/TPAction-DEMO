// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SL_BaseEnemy.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/SL_AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/GameplayTags.h"


ASL_BaseEnemy::ASL_BaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<USL_AbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<USL_AttributeSet>("AttributeSet");
}

FVector ASL_BaseEnemy::GetTargetLocation_Implementation(const FGameplayTag& InTag) const
{
	if (!IsValid(LimbsComponent) || !GetMesh()) return FVector();
	
	if (InTag.MatchesTagExact(SLTags::Limbs::Center))
	{
		return GetMesh()->DoesSocketExist(TargetSocketName) ? GetMesh()->GetSocketLocation(TargetSocketName) : GetActorLocation();
	}
	
	FCharacterLimb SelectedLimb = LimbsComponent->GetLimbByTag(InTag);
	return GetMesh()->GetBoneLocation(SelectedLimb.DisplayBoneName);
}

void ASL_BaseEnemy::SetTargetMarkerVisibility_Implementation(bool bVisible)
{
	if (TargetMarkerWidget)
	{
		TargetMarkerWidget->SetVisibility(bVisible);
	}
}

FVector ASL_BaseEnemy::GetOwnerActorLocation_Implementation() const
{
	return GetActorLocation();
}

TMap<FGameplayTag, FVector> ASL_BaseEnemy::GetTargetBonesLocations_Implementation() const
{
	if (!IsValid(LimbsComponent) || !GetMesh()) return TMap<FGameplayTag, FVector>();
	
	TArray<FCharacterLimb> Limbs = LimbsComponent->GetCharacterLimbs();
	
	TMap<FGameplayTag, FVector> TargetBones;
	for (auto& Limb : Limbs)
	{
		TargetBones.Add(Limb.LimbTag, GetMesh()->GetBoneLocation(Limb.DisplayBoneName));
	}
	
	return TargetBones;
}

FVector ASL_BaseEnemy::GetBoneLocationByTag_Implementation(const FGameplayTag& BoneTag) const
{
	if (!IsValid(LimbsComponent) || !GetMesh() || !BoneTag.IsValid()) return FVector();
	
	FCharacterLimb Limb = LimbsComponent->GetLimbByTag(BoneTag);
	return GetMesh()->GetBoneLocation(Limb.DisplayBoneName);
}

bool ASL_BaseEnemy::IsBoneValidByTag_Implementation(const FGameplayTag& BoneTag) const
{
	if (!IsValid(LimbsComponent) || !GetMesh() || !BoneTag.IsValid()) return false;
	
	return LimbsComponent->IsBoneValidByTag(BoneTag);
}


UAbilitySystemComponent* ASL_BaseEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ASL_BaseEnemy::GetAttributeSet() const
{
	return AttributeSet;
}

void ASL_BaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	
	bUseControllerRotationYaw = false;
	MoveComp->bUseControllerDesiredRotation = true;
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	if (GetAbilitySystemComponent()->GetAttributeSet(USL_AttributeSet::StaticClass()) == nullptr)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Error, no AttributeSet"));
	}
	InitializePrimaryAttributes();
	GiveStartupAbilities();
}


