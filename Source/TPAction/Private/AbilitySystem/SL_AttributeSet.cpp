// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/SL_AttributeSet.h"

#include "Components/SL_LimbsComponent.h"
#include "GameplayEffectExtension.h"

void USL_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void USL_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamageDone = GetIncomingDamage();
		
		SetIncomingDamage(0.0f);
		if (LocalDamageDone > 0.0f)
		{
			// AActor* TargetActor = Data.Target.GetAvatarActor();
			// if (!TargetActor) return;
			//
			// USL_LimbsComponent* LimbsComponent = TargetActor->GetComponentByClass<USL_LimbsComponent>();
			// float ActualDamageApplied = LocalDamageDone;
			//
			// if (LimbsComponent)
			// {
			// 	LimbsComponent->ApplyHitToLimb(Data.EffectSpec, LocalDamageDone);
			// }
			
			const float OldHealth = GetHealth();
			const float NewHealth = FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth());

			SetHealth(NewHealth);

			UE_LOG(LogTemp, Log, TEXT("Damage Applied: %f | Health: %f -> %f"), LocalDamageDone, OldHealth, NewHealth);
		}
	}
}
