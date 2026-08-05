// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SL_Weapon.generated.h"

class USL_WeaponComponent;
class UAbilitySystemComponent;

UCLASS()
class TPACTION_API ASL_Weapon : public AActor
{
	GENERATED_BODY()

public:
	
	ASL_Weapon();
	virtual void BeginPlay() override;
	void AttachToSocket(ACharacter* CallerCharacter, const FName& SocketName) const;
	void StartTracing();
	void StopTracing();
	void SetOwnerASC(UAbilitySystemComponent* InASC);
	
	USL_WeaponComponent* GetWeaponComponent() const;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	UPROPERTY()
	TWeakObjectPtr<USL_WeaponComponent> WeaponComponent;
};
