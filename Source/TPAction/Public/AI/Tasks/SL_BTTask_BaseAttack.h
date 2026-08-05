// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SL_BTTask_BaseAttack.generated.h"

struct FAbilityEndedData;
/**
 * 
 */
UCLASS()
class TPACTION_API USL_BTTask_BaseAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	
	USL_BTTask_BaseAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackTargetKey;
	
	UPROPERTY(EditAnywhere, Category = "Delay", meta = (ClampMin = "0.0"))
	float MinPostAttackDelay = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Delay", meta = (ClampMin = "0.0"))
	float MaxPostAttackDelay = 2.0f;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayAbility> AbilityToActivate;
	
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData, UBehaviorTreeComponent* OwnerComp);
	
	void OnPostAttackDelayFinished(TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompPtr);
	
	FDelegateHandle AbilityEndedDelegateHandle;

	FTimerHandle PostAttackTimerHandle;
};
