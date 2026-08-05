// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SL_WeaponComponent.generated.h"


class UGameplayAbility;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FSocketsContainer
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TArray<FName> TraceSockets;
};

USTRUCT(BlueprintType)
struct FTraceSocket
{
	GENERATED_BODY()
	
	FName SocketName;
	FVector SocketLocation;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPACTION_API USL_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USL_WeaponComponent();
	void InitializeOwnerASC(UAbilitySystemComponent* InASC);
	void ClearOwnerASC();
	void StartTrace(const FGameplayTagContainer& ActiveTraceTag);
	void StopTrace();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	void TryGetASCFromOwner();
	
	UPROPERTY()
	TSet<AActor*> AlreadyHitActors;
	
	UPROPERTY()
	TArray<AActor*> CachedIgnoreArray;
	
	TArray<FHitResult> CachedHitResults;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag HitGameplayTag;
	
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OwnerCharacterASC;
	
	// UPROPERTY(EditAnywhere)
	// TArray<FName> TraceSockets;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FSocketsContainer> TagToSockets;
	
	UPROPERTY(EditAnywhere)
	float TraceRadius = 8.f;
	
	UPROPERTY()
	TMap<FName, FVector> PreviousSocketLocations;
	
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
	
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	FGameplayTagContainer CurrentTraceTags = FGameplayTagContainer();
	
	bool bTracing = false;

	void TickTrace();
	void OnHitActor(AActor* InActor, const FHitResult& Hit);
	void HandleWeaponHit(const TArray<FHitResult>& Hits);
};
