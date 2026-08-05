// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/SL_BTTask_BaseAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

USL_BTTask_BaseAttack::USL_BTTask_BaseAttack()
{
	NodeName = "Activate Ability & Post-Attack Wait";
	bCreateNodeInstance = true;
	
	AttackTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USL_BTTask_BaseAttack, AttackTargetKey), AActor::StaticClass());
}

EBTNodeResult::Type USL_BTTask_BaseAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AbilityToActivate)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);

	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}
	
	const bool bSuccess = ASC->TryActivateAbilityByClass(AbilityToActivate);
	if (!bSuccess)
	{
		return EBTNodeResult::Failed;
	}
	
	AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &USL_BTTask_BaseAttack::OnAbilityEnded, &OwnerComp);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type USL_BTTask_BaseAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		APawn* Pawn = AIController->GetPawn();
		AIController->GetWorldTimerManager().ClearTimer(PostAttackTimerHandle);

		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
		{
			ASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);

			if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityToActivate))
			{
				ASC->CancelAbilityHandle(Spec->Handle);
			}
		}
	}

	return EBTNodeResult::Aborted;
}

void USL_BTTask_BaseAttack::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp)
{
	if (!AbilityEndedData.AbilitySpecHandle.IsValid() || AbilityEndedData.AbilityThatEnded.GetClass() != AbilityToActivate)
	{
		return;
	}

	if (!OwnerComp) return;

	AAIController* AIController = OwnerComp->GetAIOwner();
	if (!AIController) return;

	APawn* Pawn = AIController->GetPawn();
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
	{
		ASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	
	const float RandomDelay = FMath::RandRange(MinPostAttackDelay, MaxPostAttackDelay);
	
	TWeakObjectPtr<UBehaviorTreeComponent> WeakOwnerComp(OwnerComp);
    
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &USL_BTTask_BaseAttack::OnPostAttackDelayFinished, WeakOwnerComp);

	AIController->GetWorldTimerManager().SetTimer(PostAttackTimerHandle, TimerDelegate, RandomDelay, false);
}

void USL_BTTask_BaseAttack::OnPostAttackDelayFinished(TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompPtr)
{
	if (!OwnerCompPtr.IsValid()) return;

	UBehaviorTreeComponent* OwnerComp = OwnerCompPtr.Get();
	AAIController* AIController = OwnerComp->GetAIOwner();

	if (AIController)
	{
		UBlackboardComponent* BBComp = OwnerComp->GetBlackboardComponent();
		if (BBComp)
		{
			AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
			if (TargetActor)
			{
				AIController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
			}
		}
	}

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
