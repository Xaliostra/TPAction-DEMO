// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SL_WeaponComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/KismetSystemLibrary.h"


USL_WeaponComponent::USL_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USL_WeaponComponent::InitializeOwnerASC(UAbilitySystemComponent* InASC)
{
	OwnerCharacterASC = InASC;
}

void USL_WeaponComponent::ClearOwnerASC()
{
	OwnerCharacterASC = nullptr;
}

void USL_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	WeaponMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	TryGetASCFromOwner();
}

void USL_WeaponComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bTracing)
	{
		TickTrace();
	}
}

void USL_WeaponComponent::TryGetASCFromOwner()
{
	AActor* CurrentOwner = GetOwner();
	
	if (CurrentOwner && !CurrentOwner->Implements<UAbilitySystemInterface>())
	{
		if (AActor* InstigatorActor = CurrentOwner->GetInstigator())
		{
			CurrentOwner = InstigatorActor;
		}
	}
	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CurrentOwner))
	{
		OwnerCharacterASC = ASI->GetAbilitySystemComponent();
	}
	else if (IsValid(CurrentOwner))
	{
		OwnerCharacterASC = CurrentOwner->FindComponentByClass<UAbilitySystemComponent>();
	}
}

void USL_WeaponComponent::StartTrace(const FGameplayTagContainer& ActiveTraceTag)
{
	if (!IsValid(WeaponMesh)) return;
	
	AlreadyHitActors.Empty();
	
	bTracing = true;
	HitActors.Empty();
	PreviousSocketLocations.Empty();
	CurrentTraceTags = ActiveTraceTag;
	
	for (auto& Tag : ActiveTraceTag.GetGameplayTagArray())
	{
		FSocketsContainer* Container = TagToSockets.Find(Tag);
		if (!Container) continue;
		TArray<FName> TraceSockets = Container->TraceSockets;
		
		for (const FName& Socket : TraceSockets)
		{
			PreviousSocketLocations.Add(Socket, WeaponMesh->GetSocketLocation(Socket));
		}
	}
}

void USL_WeaponComponent::StopTrace()
{
	bTracing = false;
	
	CurrentTraceTags.Reset();
	PreviousSocketLocations.Empty();
	HitActors.Empty();
}

void USL_WeaponComponent::TickTrace()
{
	if (!IsValid(WeaponMesh) || !OwnerCharacterASC.IsValid()) return;

	AActor* OwnerActor = OwnerCharacterASC->GetOwner();
	if (!OwnerActor) return;

	CachedIgnoreArray.Reset();
	CachedIgnoreArray.Add(OwnerActor);
    
	for (AActor* HitActor : AlreadyHitActors)
	{
		if (IsValid(HitActor))
		{
			CachedIgnoreArray.Add(HitActor);
		}
	}
	
	for (auto& Tag : CurrentTraceTags)
	{
		FSocketsContainer* Container = TagToSockets.Find(Tag);
		if (!Container) continue;
		TArray<FName> TraceSockets = Container->TraceSockets;
		
		for (const FName& Socket : TraceSockets)
		{
			FVector* PrevLocPtr = PreviousSocketLocations.Find(Socket);
			if (!PrevLocPtr) continue;

			FVector Start = *PrevLocPtr;
			FVector End = WeaponMesh->GetSocketLocation(Socket);

			CachedHitResults.Reset();

			UKismetSystemLibrary::SphereTraceMulti(
				GetWorld(),
				Start,
				End,
				TraceRadius,
				UEngineTypes::ConvertToTraceType(ECC_Pawn),
				false,
				CachedIgnoreArray,
				EDrawDebugTrace::None,
				CachedHitResults,
				true
			);

			PreviousSocketLocations[Socket] = End;

			if (CachedHitResults.Num() > 0)
			{
				HandleWeaponHit(CachedHitResults);
			}
		}
	}
}

void USL_WeaponComponent::OnHitActor(AActor* InActor, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *InActor->GetName());
}

void USL_WeaponComponent::HandleWeaponHit(const TArray<FHitResult>& Hits)
{
	FGameplayAbilityTargetDataHandle TargetDataHandle;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor || AlreadyHitActors.Contains(HitActor))
		{
			continue;
		}

		AlreadyHitActors.Add(HitActor);
		CachedIgnoreArray.Add(HitActor);

		FGameplayAbilityTargetData_SingleTargetHit* SingleHitData = new FGameplayAbilityTargetData_SingleTargetHit();
		SingleHitData->HitResult = Hit;
		TargetDataHandle.Add(SingleHitData);
	}

	if (TargetDataHandle.Num() > 0)
	{
		FGameplayEventData Payload;
		Payload.EventTag = HitGameplayTag;
		Payload.Instigator = OwnerCharacterASC->GetOwner();
		Payload.TargetData = TargetDataHandle;

		OwnerCharacterASC->HandleGameplayEvent(HitGameplayTag, &Payload);
	}
}


