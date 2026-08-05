// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SL_DodgeAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Characters/SL_PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"

void USL_DodgeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!IsValid(BackwardMontage) || !IsValid(ForwardMontage) || !IsValid(RightMontage) || !IsValid(LeftMontage))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); 
		return;
	}
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	CachedControlRotation = Character->GetActorRotation();

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); 
		return;
	}
	
	bSavedUseControllerYaw = Character->bUseControllerRotationYaw;
	bSavedOrientRotation = Character->GetCharacterMovement()->bOrientRotationToMovement;
	bSavedDesiredRotation = Character->GetCharacterMovement()->bUseControllerDesiredRotation;
	
	Character->bUseControllerRotationYaw = false;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	
	FVector InputVector = MoveComp->GetLastInputVector();
	if (InputVector.IsNearlyZero())
	{
		InputVector = Character->GetActorForwardVector();
	}

	FVector LocalInput = Character->GetActorQuat().UnrotateVector(InputVector);
	float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(LocalInput.Y, LocalInput.X));
	
	int32 DirectionIndex = FMath::FloorToInt((AngleDeg + 45.f) / 90.f);
	
	if (DirectionIndex < 0) DirectionIndex += 4;
	if (DirectionIndex == 4) DirectionIndex = 0;
	
	float DirectionYawOffset = 0.f;
	UAnimMontage* SelectedMontage = nullptr;
	switch (DirectionIndex)
	{
	case 0:
		SelectedMontage = ForwardMontage;
		DirectionYawOffset = 0.f;
		break;
	case 1:
		SelectedMontage = RightMontage;
		DirectionYawOffset = 90.f;
		break;
	case 2:
		SelectedMontage = BackwardMontage;
		DirectionYawOffset = 180.f;
		break;
	case 3:
		SelectedMontage = LeftMontage;
		DirectionYawOffset = 270.f;
		break;
	default:
		break;
	}

	FVector CameraForward = Character->GetActorForwardVector();
	CameraForward.Z = 0.f;
	CameraForward.Normalize();

	float AnimatorErrorYaw = 45.f;
	FRotator TargetCapsuleRotation = CameraForward.Rotation();
	TargetCapsuleRotation.Yaw += AnimatorErrorYaw;

	Character->SetActorRotation(TargetCapsuleRotation);
	TargetCapsuleRotation.Yaw -= AnimatorErrorYaw;
	TargetCapsuleRotation.Yaw += DirectionYawOffset;

	FVector DashDirection = TargetCapsuleRotation.Vector(); 

	float DashDistance = 350.f;
	FVector TargetLocation = Character->GetActorLocation() + (DashDirection * DashDistance);

	UMotionWarpingComponent* WarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>();
	if (WarpingComp)
	{
		WarpingComp->AddOrUpdateWarpTargetFromLocation(TEXT("DashTarget"), TargetLocation);
	}
	
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, 
	NAME_None, 
	SelectedMontage, 
	1.5f, 
	NAME_None, 
	true // StopWhenAbilityEnds
);
	
	if (IsValid(PlayMontageTask))
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
		GrantTags();
		PlayMontageTask->ReadyForActivation();
	}
}

void USL_DodgeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}
	
	Character->bUseControllerRotationYaw = bSavedUseControllerYaw;
	Character->GetCharacterMovement()->bOrientRotationToMovement = bSavedOrientRotation;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = bSavedDesiredRotation;
	
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	
	Character->bUseControllerRotationYaw = bSavedUseControllerYaw;
	Character->GetCharacterMovement()->bOrientRotationToMovement = bSavedOrientRotation;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true; 
	
	Character->GetCharacterMovement()->RotationRate.Yaw = 540.f;
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_NavWalking);
	ReleaseTags();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_DodgeAbility::GrantTags()
{
	IAbilitySystemInterface* ASInterface = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo());
	if (!ASInterface) return;
	
	UAbilitySystemComponent* ASC = ASInterface->GetAbilitySystemComponent();
	
	for (auto Tag : TagsToGrant)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void USL_DodgeAbility::ReleaseTags()
{
	IAbilitySystemInterface* ASInterface = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo());
	if (!ASInterface) return;
	
	UAbilitySystemComponent* ASC = ASInterface->GetAbilitySystemComponent();
	
	for (auto Tag : TagsToGrant)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

void USL_DodgeAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); 
}

void USL_DodgeAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); 
}

void USL_DodgeAbility::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); 
}
