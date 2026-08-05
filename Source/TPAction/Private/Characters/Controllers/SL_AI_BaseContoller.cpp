// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Controllers/SL_AI_BaseContoller.h"

#include "BehaviorTree/BlackboardComponent.h"


ASL_AI_BaseContoller::ASL_AI_BaseContoller()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASL_AI_BaseContoller::SetNewTarget(AActor* NewTarget)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!IsValid(BB)) return;
	
	BB->SetValueAsObject(SetTargetKeyName, NewTarget);
	
	SetFocus(NewTarget);
}

void ASL_AI_BaseContoller::BeginPlay()
{
	Super::BeginPlay();
	
	RunBehaviorTree(BT);
}


