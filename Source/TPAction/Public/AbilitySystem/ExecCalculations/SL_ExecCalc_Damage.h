// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/SL_AttributeSet.h"
#include "SL_ExecCalc_Damage.generated.h"

/**
 * 
 */

struct FSL_DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	
	FSL_DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USL_AttributeSet, IncomingDamage, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USL_AttributeSet, Armor, Target, false);
	}
};

static const FSL_DamageStatics& DamageStatics()
{
	static FSL_DamageStatics DStatics;
	return DStatics;
}

UCLASS()
class TPACTION_API USL_ExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	
	USL_ExecCalc_Damage();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
