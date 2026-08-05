// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalculations/SL_ExecCalc_Damage.h"

#include "Components/SL_LimbsComponent.h"
#include "Tags/GameplayTags.h"

USL_ExecCalc_Damage::USL_ExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void USL_ExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	if (TargetTags->HasTag(SLTags::States::IsDodging))
	{
		return;
	}
	
	AActor* TargetActor = TargetASC->GetOwner();
	if (!TargetActor) return;
	
	USL_LimbsComponent* LimbsComponent = TargetActor->GetComponentByClass<USL_LimbsComponent>();
	if (!LimbsComponent) return;
	
	float BaseDamage = FMath::Max<float>(Spec.GetSetByCallerMagnitude(SLTags::Abilities::Effects::Damage, false, 0.0f), 0.0f);
	float Armor = LimbsComponent->GetLimbArmor(Spec);
	Armor = FMath::Max<float>(Armor, 0.0f);
	
	float LimbMultiplier = 1.0f;
	LimbMultiplier = LimbsComponent->GetLimbMultiplicator(Spec);
	float MitigatedDamage = (BaseDamage * LimbMultiplier) - Armor;
	float FinalDamage = FMath::Max<float>(MitigatedDamage, 0.0f);
	
	LimbsComponent->ApplyHitToLimb(Spec, -FinalDamage);
	
	if (FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics().IncomingDamageProperty, 
				EGameplayModOp::Additive, 
				FinalDamage
			)
		);
	}
}
