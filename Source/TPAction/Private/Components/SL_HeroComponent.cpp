// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SL_HeroComponent.h"

#include "AbilitySystemInterface.h"


USL_HeroComponent::USL_HeroComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

UAbilitySystemComponent* USL_HeroComponent::GetAbilitySystemComponent() const
{
	return AbilitySystem;	
}

void USL_HeroComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* Pawn = GetPawn();
	
	if (!Pawn) return;
	
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Pawn))
	{
		AbilitySystem = Interface->GetAbilitySystemComponent();
		
		OnAbilitySystemInitialized.Broadcast(AbilitySystem);
	}
}

APawn* USL_HeroComponent::GetPawn() const
{
	return Cast<APawn>(GetOwner());
}
