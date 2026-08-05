// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SL_LimbsComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Engine/StaticMeshActor.h"
#include "Tags/GameplayTags.h"
#include "UI/SL_HUD.h"


USL_LimbsComponent::USL_LimbsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USL_LimbsComponent::CacheLimbBones()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	
	USkeletalMeshComponent* OwnerMesh = Owner->GetComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(OwnerMesh)) return;
	
	auto* SkeletalMesh = OwnerMesh->GetSkeletalMeshAsset();
	if (!IsValid(SkeletalMesh)) return;
	
	const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetRefSkeleton();
	int32 NumBones = RefSkeleton.GetNum();
	
	for (const auto& LimbCache : Limbs)
	{
		if (LimbCache.Key == SLTags::Limbs::Center) continue; //This 'limb' serves general display purposes and is not meant to bind to any of own children
		
		for (int32 i = 0; i < NumBones; ++i)
		{
			FName CurrentBoneName = RefSkeleton.GetBoneName(i);
			
			if (CurrentBoneName == LimbCache.Value.BoneName || OwnerMesh->BoneIsChildOf(CurrentBoneName, LimbCache.Value.BoneName))
			{
				LimbBonesCache.FindOrAdd(CurrentBoneName) = LimbCache.Value.LimbTag;
			}
		}
	}
}

void USL_LimbsComponent::ApplyHitToLimb(const FGameplayEffectSpec& ReceivedSpec, float FinalDamage)
{
	FCharacterLimb* CurrentLimb = GetLimbFromSpec(ReceivedSpec);
	if (!CurrentLimb || CurrentLimb->IsDestroyed) return;
		
	// FGameplayTagContainer AssetTags;
	// ReceivedSpec.GetAllAssetTags(AssetTags);
	// if (!AssetTags.HasAny(ExpectedEffectTags)) return;
	
	CurrentLimb->CurrentHealth += FinalDamage;
	if (CurrentLimb->CurrentHealth < 0.1f)
	{
		//TODO: Apply OnLimbDestroyed()
		HandleLimbDestruction(CurrentLimb->LimbTag);
	}
	//TODO: Apply FeedbackInstigator()
	
	//TODO: Notify LimbHealthWidget
	OnLimbStateChanged.Broadcast(CurrentLimb->LimbTag, CurrentLimb->MaxHealth, CurrentLimb->CurrentHealth);
}

float USL_LimbsComponent::GetLimbArmor(const FGameplayEffectSpec& ReceivedSpec)
{
	FCharacterLimb* CurrentLimb = GetLimbFromSpec(ReceivedSpec);
	if (!CurrentLimb) return 0.f;
	
	return CurrentLimb->Armor;
}

float USL_LimbsComponent::GetLimbMultiplicator(const FGameplayEffectSpec& ReceivedSpec)
{
	FCharacterLimb* CurrentLimb = GetLimbFromSpec(ReceivedSpec);
	if (!CurrentLimb) return 1.f;
	
	return CurrentLimb->LimbMultiplier;
}

FName USL_LimbsComponent::GetLimbNameByTag(const FGameplayTag& InTag) const
{
	return FName();
}

FCharacterLimb USL_LimbsComponent::GetLimbByTag(const FGameplayTag& InTag) const
{
	return Limbs[InTag];
}


TArray<FCharacterLimb> USL_LimbsComponent::GetCharacterLimbs() const
{
	TArray<FCharacterLimb> RetLimbs;
	for (const auto& Limb : Limbs)
	{
		RetLimbs.Add(Limb.Value);
	}
	return RetLimbs;
}

bool USL_LimbsComponent::IsBoneValidByTag(const FGameplayTag& InTag) const
{
	auto CurrentLimb = Limbs.Find(InTag);
	if (!CurrentLimb) return false;
	
	if (CurrentLimb->CurrentHealth < 0.1f) return false;
	
	//Any other validation;
	
	return true;
}

void USL_LimbsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CacheLimbBones();
}

void USL_LimbsComponent::HandleLimbDestruction(const FGameplayTag& InTag)
{
	auto CurrentLimb = Limbs.Find(InTag);
	
	if (!CurrentLimb || !CurrentLimb->DissectedLimb_Mesh || !CurrentLimb->LimbWound_Mesh) return;
	
	CurrentLimb->IsDestroyed = true;
	
	if (CurrentLimb->bIsSupporting)
	{
		
	}
	else
	{
		DissectTheLimb(*CurrentLimb);
	}
}

void USL_LimbsComponent::DissectTheLimb(const FCharacterLimb& InLimb)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	
	USkeletalMeshComponent* OwnerMesh = Owner->GetComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(OwnerMesh)) return;
	
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(GetOwner());
	if (IsValid(StaticMeshComponent))
	{
		StaticMeshComponent->RegisterComponent();
		StaticMeshComponent->SetStaticMesh(InLimb.LimbWound_Mesh);
		
		StaticMeshComponent->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InLimb.BoneName);
		StaticMeshComponent->SetWorldScale3D(FVector(0.2f));
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(InstigatorASC.Get()->GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	FTransform BoneTransform = OwnerMesh->GetBoneTransform(InLimb.BoneName);
	
	AStaticMeshActor* LimbActor = GetWorld()->SpawnActor<AStaticMeshActor>(
			BoneTransform.GetLocation(), 
			FRotator(0.0f, 0.0f, 0.0f), 
			SpawnParams
		);
	
	if (LimbActor && LimbActor->GetStaticMeshComponent())
	{
		UStaticMeshComponent* LimbComp = LimbActor->GetStaticMeshComponent();
		LimbComp->SetMobility(EComponentMobility::Movable);
		LimbComp->SetStaticMesh(InLimb.DissectedLimb_Mesh);
		
		LimbComp->SetSimulatePhysics(true);
		LimbComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		FVector Impulse = BoneTransform.GetRotation().GetForwardVector() * 300.0f + FVector(0, 0, 200.0f);
		LimbComp->AddImpulse(Impulse, NAME_None, true);
	}
	
	
	OwnerMesh->HideBoneByName(InLimb.BoneName, PBO_Term);
}


FCharacterLimb* USL_LimbsComponent::GetLimbFromSpec(const FGameplayEffectSpec& ReceivedSpec)
{
	InstigatorASC = ReceivedSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
	
	FGameplayEffectContextHandle ContextHandle = ReceivedSpec.GetEffectContext();
	
	const FHitResult* HitResult = ContextHandle.GetHitResult();
	if (!HitResult) return nullptr;
	
	FGameplayTag* FoundLimbTag = LimbBonesCache.Find(HitResult->BoneName);
	if (!FoundLimbTag) return nullptr;
	
	return Limbs.Find(*FoundLimbTag);
}


