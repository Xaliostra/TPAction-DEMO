// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SL_PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystem/SL_AttributeSet.h"
#include "Characters/SL_BaseEnemy.h"
#include "Characters/Controllers/SL_AI_BaseContoller.h"
#include "Characters/Items/SL_Weapon.h"
#include "Interfaces/Targeting/TargetableInterface.h"
#include "UI/SL_HUD.h"


ASL_PlayerCharacter::ASL_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(200.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = BaseTargetArmLength;
	CameraBoom->bUsePawnControlRotation = true;
	
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = DefaultRotationLagSpeed;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<USL_AbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<USL_AttributeSet>("AttributeSet");
}

void ASL_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	if (GetAbilitySystemComponent()->GetAttributeSet(USL_AttributeSet::StaticClass()) == nullptr)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Error, no AttributeSet"));
	}
	InitializePrimaryAttributes();
	InitializeAdditionalGrantedEffects();
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	bIsAbilitySystemInitialized = true;
	GiveStartupAbilities();
	BindEventsToASC();
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	ASLHUD = Cast<ASL_HUD>(PC->GetHUD());
	
	SpawnAndEquipWeapon();
}

void ASL_PlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	bool bIsDashing = AbilitySystemComponent->HasMatchingGameplayTag(IsDodgingTag);
	float TargetFOV = bIsDashing ? DashFOV : DefaultFOV;
	if ((FollowCamera && bIsDashing) || FollowCamera->FieldOfView != DefaultFOV)
	{
		float CurrentFOV = FollowCamera->FieldOfView;
		float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, FOVInterpSpeed);
		
		FollowCamera->SetFieldOfView(NewFOV);
	}
	
	UpdateCamera_Locked(DeltaSeconds);
}

UAbilitySystemComponent* ASL_PlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ASL_PlayerCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ASL_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ASL_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ASL_PlayerCharacter::BindEventsToASC()
{
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(MovementLockedTag).AddUObject(this, &ThisClass::ToggleMovement);
}

void ASL_PlayerCharacter::SpawnAndEquipWeapon()
{
	if (!WeaponToEquipClass) return;
	if (IsValid(CurrentEquippedWeapon)) return; //Weapon Already Spawned And Equipped, switching is not implemented yet
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ASL_Weapon* SpawnedWeapon = GetWorld()->SpawnActor<ASL_Weapon>(WeaponToEquipClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (IsValid(SpawnedWeapon))
	{
		CurrentEquippedWeapon = SpawnedWeapon;
		CurrentEquippedWeapon->AttachToSocket(this, RightWeaponSocketName);
		CurrentEquippedWeapon->SetOwnerASC(GetAbilitySystemComponent());
	}
}

void ASL_PlayerCharacter::UpdateCamera_Locked(float DeltaSeconds)
{
	if (!CurrentTarget || !CameraBoom || !FollowCamera) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    FVector CharLoc = GetActorLocation();
    FVector TargetLoc = ITargetableInterface::Execute_GetTargetLocation(CurrentTarget.GetObject(), SelectedEnemyLimbTag);
	
    FVector CharToTargetDir = (TargetLoc - CharLoc).GetSafeNormal();
    float TargetYaw = CharToTargetDir.Rotation().Yaw;
	
    FVector CameraLoc = FollowCamera->GetComponentLocation();
    FVector CamToTargetDir = (TargetLoc - CameraLoc).GetSafeNormal();
    float RawPitch = CamToTargetDir.Rotation().Pitch;

    float ClampedPitch = FMath::Clamp(RawPitch, MinPitchDown, MaxPitchUp);

    float CalculatedZOffset = BaseCameraHeight;

    if (RawPitch < MinPitchDown)
    {
        float HorizontalDist = FVector::Dist2D(CharLoc, TargetLoc);
        float TargetRelativeZ = TargetLoc.Z - CharLoc.Z;

        float AngleRad = FMath::DegreesToRadians(FMath::Abs(MinPitchDown));
        float RequiredZ = TargetRelativeZ + (HorizontalDist * FMath::Tan(AngleRad));

        CalculatedZOffset = FMath::Clamp(RequiredZ, BaseCameraHeight, MaxCameraHeight);
    }

	FVector CurrentTargetOffset = CameraBoom->TargetOffset;
    FVector DesiredTargetOffset = FVector(0.0f, 0.0f, CalculatedZOffset);
    CameraBoom->TargetOffset = FMath::VInterpTo(CurrentTargetOffset, DesiredTargetOffset, DeltaSeconds, 4.0f);
	
    float Distance = FVector::Dist(CharLoc, TargetLoc);
    float ProximityFactor = 1.0f - FMath::Clamp((Distance - 200.0f) / BaseTargetArmLength, 0.0f, 1.0f);
	
    float DesiredArmLength = FMath::Lerp(BaseTargetArmLength, BaseTargetArmLength + 200.f, ProximityFactor);
    CameraBoom->TargetArmLength = FMath::FInterpTo(BaseTargetArmLength, DesiredArmLength, DeltaSeconds, 2.0f);

    FRotator CurrentRotation = PC->GetControlRotation();
    FRotator DesiredRotation = FRotator(ClampedPitch, TargetYaw, CurrentRotation.Roll);

    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaSeconds, 5.0f);
    PC->SetControlRotation(NewRotation);

    if (Distance > TargetRadius || !ITargetableInterface::Execute_IsTargetable(CurrentTarget.GetObject()))
    {
        SetTargetLockActive(false);
        ClearTarget();
    }
}

void ASL_PlayerCharacter::SetTargetLockActive(bool bIsLocked)
{
	bIsLockedOn = bIsLocked;
	if (!CameraBoom) return;

	if (bIsLocked)
	{
		CameraBoom->CameraRotationLagSpeed = LockOnRotationLagSpeed;

		CameraBoom->SocketOffset.Y = 0.0f; 
	}
	else
	{
		CameraBoom->CameraRotationLagSpeed = DefaultRotationLagSpeed;
		CameraBoom->TargetOffset = FVector(0.0f, 0.0f, BaseCameraHeight);
		CameraBoom->TargetArmLength = 350.0f;
	}
}

void ASL_PlayerCharacter::FindTarget()
{
	if (CurrentTarget != nullptr)
	{
		ClearTarget();
		bIsLockedOn = false;
		return;
	}
	
	FVector StartLoc = GetActorLocation();
	TArray<FHitResult> HitResults;
	FComponentQueryParams Params;
	Params.AddIgnoredActor(this);
	
	GetWorld()->SweepMultiByChannel(HitResults, StartLoc, StartLoc, FQuat::Identity, 
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(TargetRadius), Params
		);
	
	AActor* BestTarget = nullptr;
	float BestScore = BIG_NUMBER;
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	
	int32 ViewportWidth, ViewportHeight;
	PC->GetViewportSize(ViewportWidth, ViewportHeight);
	FVector2D ScreenCenter(ViewportWidth / 2.f, ViewportHeight / 2.f);
	
	for (const FHitResult& Hit : HitResults)
	{
		AActor* PotentialTarget = Hit.GetActor();
		if (!IsActorValidTarget(PotentialTarget)) continue;
		
		FHitResult VisibilityHit;
		FCollisionQueryParams TraceParams(FName(TEXT("Visibility")), true, this);
		if (GetWorld()->LineTraceSingleByChannel(VisibilityHit, GetMesh()->GetSocketLocation("Head"), PotentialTarget->GetActorLocation(), ECC_Visibility, TraceParams))
		{
			if (VisibilityHit.GetActor() != PotentialTarget) continue;
		}
		
		FVector2D ScreenPosition;
		if (PC->ProjectWorldLocationToScreen(PotentialTarget->GetActorLocation(), ScreenPosition))
		{
			float DistanceToCenter = FVector2D::Distance(ScreenPosition, ScreenCenter);
			
			if (BestTarget == nullptr || DistanceToCenter < BestScore)
			{
				BestScore = DistanceToCenter;
				BestTarget = PotentialTarget;
			}
		}
	}
	
	if (BestTarget)
	{
		CurrentTarget = BestTarget;
		bIsLockedOn = true;
		SetTargetLockActive(true);
		ITargetableInterface::Execute_SetTargetMarkerVisibility(CurrentTarget.GetObject(), true);
		
		//Following code sets the player as the target for the enemy
		//Normally it should be invoked by some kind of triggers
		//Yet here it's not implemented yet, so the following code remains
		
		ASL_BaseEnemy* CurrentEnemy = Cast<ASL_BaseEnemy>(BestTarget);
		if (IsValid(CurrentEnemy))
		{
			ASL_AI_BaseContoller* AIC = Cast<ASL_AI_BaseContoller>(CurrentEnemy->GetController());
			if (IsValid(AIC))
			{
				AIC->SetNewTarget(this);
			}
		}
		
		
		//if Target is a Boss check
		
		if (ASL_BaseCharacter* CharacterTarget = Cast<ASL_BaseCharacter>(CurrentTarget.GetObject()); IsValid(CharacterTarget) && ASLHUD)
		{
			ASLHUD->UpdateTargetASC(CharacterTarget->GetAbilitySystemComponent());
			ASLHUD->SetupTargetWidgets(CharacterTarget, PC);
		}
		
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
}

TScriptInterface<ITargetableInterface> ASL_PlayerCharacter::GetCurrentTarget() const
{
	return CurrentTarget;
}

void ASL_PlayerCharacter::SetNewLimbTarget(const FGameplayTag& NewLimbTag)
{
	SelectedEnemyLimbTag = NewLimbTag;
	if (!ASLHUD) return;
	ASLHUD->SetTargetWidgetSelected(NewLimbTag);
}

void ASL_PlayerCharacter::ClearTarget()
{
	if (CurrentTarget)
	{
		if (ASL_BaseCharacter* CharacterTarget = Cast<ASL_BaseCharacter>(CurrentTarget.GetObject()); IsValid(CharacterTarget))
			ASLHUD->UnbindFromTarget(CharacterTarget);
		ITargetableInterface::Execute_SetTargetMarkerVisibility(CurrentTarget.GetObject(), false);
		CurrentTarget = nullptr;
	}
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void ASL_PlayerCharacter::InterpolateCamera(const float DeltaTime)
{
	if (CurrentTarget)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FVector TargetLoc = ITargetableInterface::Execute_GetTargetLocation(CurrentTarget.GetObject(), SelectedEnemyLimbTag);
			FVector CameraLoc = FollowCamera->GetComponentLocation();
			
			FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetLoc - CameraLoc).Rotator();
			
			FRotator CurrentRotation = PC->GetControlRotation();
			TargetRotation.Roll = CurrentRotation.Roll; 

			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);

			PC->SetControlRotation(NewRotation);
		}
		
		if (FVector::Dist(GetActorLocation(), ITargetableInterface::Execute_GetOwnerActorLocation(CurrentTarget.GetObject())) > TargetRadius 
			|| !ITargetableInterface::Execute_IsTargetable(CurrentTarget.GetObject()))
		{
			ClearTarget();
		}
	}
}

bool ASL_PlayerCharacter::IsActorValidTarget(AActor* InActor) const
{
	return InActor && InActor->GetClass()->ImplementsInterface(UTargetableInterface::StaticClass()) 
		&& ITargetableInterface::Execute_IsTargetable(InActor);
}

void ASL_PlayerCharacter::ToggleMovement(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag.MatchesTagExact(MovementLockedTag))
	{
		bool bBlockMovement = (NewCount > 0);
		
		if (AController* PC = GetController())
		{
			PC->SetIgnoreMoveInput(bBlockMovement);
		}
	}
}

FLocomotionSelection ASL_PlayerCharacter::GetLocomotionSelection() const
{
	FVector Velocity = GetMovementComponent()->Velocity;
	FVector Forward = GetActorForwardVector();
	
	Velocity.Normalize();
	Forward.Normalize();
	
	float Dot = FVector::DotProduct(Forward, Velocity);
	float Cross = FVector::CrossProduct(Forward, Velocity).Z;
	
	float ACos = FMath::Acos(Dot);
	ACos = FMath::RadiansToDegrees(ACos);
	float Angle = ACos * FMath::Sign(Cross);
	
	int32 AnimID = 0;
	float absAngle = FMath::Abs(Angle);

	if (absAngle < 55) 
		AnimID = 2;
	else if (absAngle > 125) 
		AnimID = 0;
	else 
		AnimID = Angle < 0 ? 1 : 3;
	
	float BaseAngle = 0.f;

	if (AnimID == 3)      BaseAngle = 90.f;
	else if (AnimID == 1) BaseAngle = -90.f;
	else if (AnimID == 0) BaseAngle = (Angle < 0.f) ? -180.f : 180.f; 

	float WarpAngle = Angle - BaseAngle;
	
	return {AnimID, WarpAngle};
}

void ASL_PlayerCharacter::OnPrimaryAttack()
{
	FGameplayTagContainer TargetTags;
	TargetTags.AddTag(PrimaryAbilityTag);
	
 	bool bActivated = AbilitySystemComponent->TryActivateAbilitiesByTag(TargetTags, true);
	
	if (!bActivated)
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;
		
		FGameplayTag InputEventTag = PrimaryInputTag;
		AbilitySystemComponent->HandleGameplayEvent(InputEventTag, &Payload);
	}
}

void ASL_PlayerCharacter::OnDodge()
{
	FGameplayTagContainer TargetTags;
	TargetTags.AddTag(DodgeAbilityTag);
	
	bool bActivated = AbilitySystemComponent->TryActivateAbilitiesByTag(TargetTags, true);
	
	if (!bActivated)
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;
		
		// FGameplayTag InputEventTag = PrimaryInputTag;
		// AbilitySystemComponent->HandleGameplayEvent(InputEventTag, &Payload);
	}
}


