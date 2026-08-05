// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MeleeCombatInterface.generated.h"

class ASL_Weapon;

// This class does not need to be modified.
UINTERFACE()
class UMeleeCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPACTION_API IMeleeCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	ASL_Weapon* GetEquippedWeapon() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void StartMeleeAttackTracing(const FGameplayTagContainer& ActiveTraceTag) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void StopMeleeAttackTracing() const;
};
