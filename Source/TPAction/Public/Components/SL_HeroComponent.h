// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SL_HeroComponent.generated.h"

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemInitialized, UAbilitySystemComponent*)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPACTION_API USL_HeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USL_HeroComponent();
	
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	FOnAbilitySystemInitialized OnAbilitySystemInitialized;
	
protected:

	virtual void BeginPlay() override;
	APawn* GetPawn() const;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;
	
};
