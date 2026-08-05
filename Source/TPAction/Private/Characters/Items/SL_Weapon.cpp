// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Items/SL_Weapon.h"

#include "Components/SL_WeaponComponent.h"
#include "GameFramework/Character.h"


ASL_Weapon::ASL_Weapon()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	
	RootComponent = WeaponMesh;
}

void ASL_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponComponent = GetComponentByClass<USL_WeaponComponent>();
}

void ASL_Weapon::AttachToSocket(ACharacter* CallerCharacter, const FName& SocketName) const
{
	if (!IsValid(CallerCharacter)) return;
	
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->IgnoreActorWhenMoving(Cast<AActor>(CallerCharacter), true);
	
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(CallerCharacter->GetMesh(), TransformRules, SocketName);
}

void ASL_Weapon::StartTracing()
{
	if (!WeaponComponent.IsValid()) return;
	//WeaponComponent->StartTrace();
}

void ASL_Weapon::StopTracing()
{
	if (!WeaponComponent.IsValid()) return;
	WeaponComponent->StopTrace();
}

void ASL_Weapon::SetOwnerASC(UAbilitySystemComponent* InASC)
{
	if (!WeaponComponent.IsValid()) return;
	WeaponComponent->InitializeOwnerASC(InASC);
}

USL_WeaponComponent* ASL_Weapon::GetWeaponComponent() const
{
	return WeaponComponent.Get();
}



