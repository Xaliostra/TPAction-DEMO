// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "SL_AI_BaseContoller.generated.h"

UCLASS()
class TPACTION_API ASL_AI_BaseContoller : public AAIController
{
	GENERATED_BODY()

public:

	ASL_AI_BaseContoller();
	void SetNewTarget(AActor* NewTarget);
	
protected:

	virtual void BeginPlay() override;

private:
	
	UPROPERTY(EditAnywhere, Category = "BehaviourTree")
	TObjectPtr<UBehaviorTree> BT;
	
	UPROPERTY(EditAnywhere, Category = "BlackBoard|Keys")
	FName SetTargetKeyName;
};
