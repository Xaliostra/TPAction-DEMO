// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SL_BlueprintLibrary.generated.h"

/**
 * 
 */
class UGameplayEffect;
struct FGameplayEventData;

UCLASS()
class TPACTION_API USL_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static void SendDesiredEventToTarget(AActor* Target, const TSubclassOf<UGameplayEffect>& DesiredEffect, const FGameplayEventData& Payload, const FGameplayTag& DataTag, float EffectMagnitude);
	
};
