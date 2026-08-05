// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SL_PrimaryAttackAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SL_AttributeSet.h"
#include "Interfaces/Combat/MeleeCombatInterface.h"


USL_PrimaryAttackAbility::USL_PrimaryAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_PrimaryAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!ComboMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (ComboSections.IsEmpty())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ResetCombo();
	ApplyOnHitToSelf();
	BindGameplayEvents();
	PlayCombo();
}

void USL_PrimaryAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetCombo();
	
	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}
	UnbindGameplayEvents();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_PrimaryAttackAbility::ApplyOnHitToSelf()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (IsValid(ASC))
	{
		for (auto Effect : EffectsApplyToSelf)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1.f, ContextHandle);
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void USL_PrimaryAttackAbility::ApplyOnHitToTarget(const TArray<AActor*>& Targets, const TArray<FHitResult>& TargetHits)
{	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	TMap<AActor*, TArray<FHitResult>> ActorToHitsMap;
	for (const FHitResult& Hit : TargetHits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			ActorToHitsMap.FindOrAdd(HitActor).Add(Hit);
		}
	}
	
	if (IsValid(ASC))
	{
		for (auto Target : Targets)
		{
			if (!IsValid(Target) || !ActorToHitsMap.Contains(Target)) continue;
			
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			if (!IsValid(TargetASC)) continue;
			
			TArray<FHitResult> Hits = ActorToHitsMap[Target];
			
			for (auto Effect : EffectsApplyToTarget)
			{
				if (!Effect) continue;
				
				for (auto Hit : Hits)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddHitResult(Hit);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1.f, ContextHandle);
					
					float BaseDamageValue = 10.f;
					//TODO: Get BaseDamage from Instigator weapon
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, BaseDamageValue);
					
					if (!SpecHandle.IsValid()) continue;
					ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					const USL_AttributeSet* AS = TargetASC->GetSet<USL_AttributeSet>();
					if (AS)
					{
						float CurrentHealth = AS->GetHealth();
					}
				}
			}
		}
	}
}

void USL_PrimaryAttackAbility::OnComboInputReceived(const FGameplayEventData* Payload)
{
	if (bComboWindowOpen)
	{
		bAttackQueued = true;
	}
}

void USL_PrimaryAttackAbility::OnTracingEvent(FGameplayEventData Payload)
{
	FGameplayTag EventTag = Payload.EventTag;
	
	if (EventTag.MatchesTagExact(StartTracingTag))
	{
		OnTracingStartedReceived(Payload);
	}
	else if (EventTag.MatchesTagExact(StopTracingTag))
	{
		OnTracingStoppedReceived(Payload);
	}
	else if (EventTag.MatchesTagExact(TargetHitTag))
	{
		OnTargetHit(Payload);
	}
}

void USL_PrimaryAttackAbility::OnTracingStartedReceived(FGameplayEventData Payload)
{
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!IsValid(Instigator)) return;
	
	if (Instigator->Implements<UMeleeCombatInterface>() && CurrentSectionActiveHand.IsValidIndex(CurrentComboIndex))
	{
		IMeleeCombatInterface::Execute_StartMeleeAttackTracing(Instigator, CurrentSectionActiveHand[CurrentComboIndex]);
	}
}

void USL_PrimaryAttackAbility::OnTracingStoppedReceived(FGameplayEventData Payload)
{
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!IsValid(Instigator)) return;
	
	if (Instigator->Implements<UMeleeCombatInterface>())
	{
		IMeleeCombatInterface::Execute_StopMeleeAttackTracing(Instigator);
	}
}

void USL_PrimaryAttackAbility::OnTargetHit(FGameplayEventData Payload)
{
	FGameplayAbilityTargetDataHandle TargetData = Payload.TargetData;
	TArray<AActor*> HitActors;
	TArray<FHitResult> HitResults;
	
	for (int32 i = 0; i < TargetData.Num(); ++i)
	{
		const FGameplayAbilityTargetData* RawData = TargetData.Get(i);
		
		if (RawData)
		{
			const FHitResult* HitResult = RawData->GetHitResult();
			
			if (HitResult)
			{
				HitActors.Add(HitResult->GetActor());
				HitResults.Add(*HitResult);
			}
		}
	}
	
	ApplyOnHitToTarget(HitActors, HitResults);
}

void USL_PrimaryAttackAbility::PlayCombo()
{
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage, PlayRate);
	
	if (!MontageTask)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	
	MontageTask->ReadyForActivation();
	JumpToCurrentSection();
}

void USL_PrimaryAttackAbility::JumpToCurrentSection()
{
	if (!ComboSections.IsValidIndex(CurrentComboIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && MontageTask)
	{
		ASC->CurrentMontageJumpToSection(ComboSections[CurrentComboIndex]);
	}
}

void USL_PrimaryAttackAbility::BindGameplayEvents()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (!ASC) return;
	
	ComboOpenHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboOpenTag).AddUObject(this, &USL_PrimaryAttackAbility::OnComboWindowOpened);
	ComboCommitHandle  = ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboCommitTag).AddUObject(this, &USL_PrimaryAttackAbility::OnComboCommit);
	ComboResetHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboResetTag).AddUObject(this, &USL_PrimaryAttackAbility::OnComboReset);
	
	ComboInputHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboInputTag).AddUObject(this, &USL_PrimaryAttackAbility::OnComboInputReceived);

	UAbilityTask_WaitGameplayEvent* WaitForTracingTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, 
		EventTracingTag, 
		nullptr, 
		false,
		false 
	);
	
	if (WaitForTracingTask)
	{
		WaitForTracingTask->EventReceived.AddDynamic(this, &USL_PrimaryAttackAbility::OnTracingEvent);
		WaitForTracingTask->ReadyForActivation();
	}
}

void USL_PrimaryAttackAbility::UnbindGameplayEvents()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (!ASC) return;
	
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboOpenTag).Remove(ComboOpenHandle);
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboCommitTag).Remove(ComboCommitHandle);
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboResetTag).Remove(ComboResetHandle);
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ComboInputTag).Remove(ComboInputHandle);
	
}

void USL_PrimaryAttackAbility::ResetCombo()
{
	CurrentComboIndex = 0;
	bAttackQueued = false;
	bComboWindowOpen = false;
}

void USL_PrimaryAttackAbility::OnComboWindowOpened(const FGameplayEventData* Payload)
{
	bComboWindowOpen = true;
}

void USL_PrimaryAttackAbility::OnComboCommit(const FGameplayEventData* Payload)
{
	bComboWindowOpen = false;
	
	if (!bAttackQueued && !bAutoProgressCombo) return;
	
	if (!CheckCost(CurrentSpecHandle, CurrentActorInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	ApplyOnHitToSelf();
	ApplyCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	
	bAttackQueued = false;
	++CurrentComboIndex;
	
	if (!ComboSections.IsValidIndex(CurrentComboIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	JumpToCurrentSection();
}

void USL_PrimaryAttackAbility::OnComboReset(const FGameplayEventData* Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_PrimaryAttackAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_PrimaryAttackAbility::OnMontageCancelled()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void USL_PrimaryAttackAbility::OnMontageInterrupted()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
