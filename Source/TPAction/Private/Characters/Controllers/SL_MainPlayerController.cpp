// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Controllers/SL_MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/SL_PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Interfaces/Targeting/TargetableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/UI/SL_UISubsystem.h"
#include "UI/SL_HUD.h"

void ASL_MainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!IsValid(InputSubsystem)) return;
	for (UInputMappingContext* Context : InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(Context, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInputComponent)) return;
	
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::SL_Dodge);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::SL_StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::SL_Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::SL_Look);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ThisClass::SL_Primary);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::SL_Secondary);
	EnhancedInputComponent->BindAction(TertiaryAction, ETriggerEvent::Started, this, &ThisClass::SL_Tertiary);
	EnhancedInputComponent->BindAction(EquipWeaponAction, ETriggerEvent::Started, this, &ThisClass::SL_EquipWeapon);
	EnhancedInputComponent->BindAction(ToggleTargetAction, ETriggerEvent::Started, this, &ThisClass::SL_TargetToggle);
	
}

void ASL_MainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (ASL_PlayerCharacter* PCharacter = Cast<ASL_PlayerCharacter>(InPawn); IsValid(PCharacter))
	{
		if (PCharacter->bIsAbilitySystemInitialized)
		{
			OnASCInitialized(PCharacter->GetAbilitySystemComponent(), PCharacter->GetAttributeSet());
			return;
		}
		
		PCharacter->OnASCInitialized.AddUObject(this, &ThisClass::OnASCInitialized);
	}
}

void ASL_MainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		ASL_MainHUD = Cast<ASL_HUD>(GetHUD());
		if (ASL_MainHUD)
		{
			ASL_MainHUD->CreateMainHUD();
		}
	}
}

void ASL_MainPlayerController::ProcessMouseFlick(const FVector2D& MouseDelta)
{
	if (MouseDelta.SizeSquared() < FMath::Square(FlickThreshold)) return;
	
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	
	if (!PlayerCharacter->GetCurrentTarget()) return;
	
	FVector2D FlickDir = MouseDelta.GetSafeNormal();
	
	FVector2D CurrentLimbScreenPos;
	if (!GetLimbScreenPosition(CurrentLimbTag, CurrentLimbScreenPos)) return;
	
	FGameplayTag BestCandidateTag = CurrentLimbTag;
	float BestScore = -1.f;
	
	for (const auto& Pair : ITargetableInterface::Execute_GetTargetBonesLocations(PlayerCharacter->GetCurrentTarget().GetObject()))
	{
		const FGameplayTag& CandidateTag = Pair.Key;
		
		if (CandidateTag.MatchesTagExact(CurrentLimbTag)) continue;
		
		FVector2D CandidateScreenPos;
		if (!GetLimbScreenPosition(CandidateTag, CandidateScreenPos)) continue;
		
		FVector2D ToCandidate = (CandidateScreenPos - CurrentLimbScreenPos);
		float Distance = ToCandidate.Size();
		
		if (Distance < 1.f) continue;
		
		FVector2D ToCandidateDir = ToCandidate / Distance;
		
		float DirectionDot = FVector2D::DotProduct(FlickDir, ToCandidateDir);
		
		if (DirectionDot > MinDirectionDot)
		{
			float Score = DirectionDot / (Distance * 0.01f);
			if (Score > BestScore 
				&& ITargetableInterface::Execute_IsBoneValidByTag(PlayerCharacter->GetCurrentTarget().GetObject(), CandidateTag)
				)
			{
				BestScore = Score;
				BestCandidateTag = CandidateTag;
			}
		}
	}
	
	if (!BestCandidateTag.MatchesTagExact(CurrentLimbTag))
	{
		CurrentLimbTag = BestCandidateTag;
		PlayerCharacter->SetNewLimbTarget(CurrentLimbTag);
	}
}

bool ASL_MainPlayerController::GetLimbScreenPosition(const FGameplayTag& LimbTag, FVector2D& OutScreenPos)
{
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn());
	auto CurrentTarget = PlayerCharacter->GetCurrentTarget();
	if (!PlayerCharacter || !CurrentTarget) return false;

	FVector WorldLoc = ITargetableInterface::Execute_GetBoneLocationByTag(CurrentTarget.GetObject(), LimbTag);
	return UGameplayStatics::ProjectWorldToScreen(this, WorldLoc, OutScreenPos, true);
}

void ASL_MainPlayerController::SL_Jump()
{
	if (!IsValid(GetCharacter())) return;

	GetCharacter()->Jump();
}

void ASL_MainPlayerController::SL_Dodge()
{
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	
	PlayerCharacter->OnDodge();
}

void ASL_MainPlayerController::SL_StopJumping()
{
	if (!IsValid(GetCharacter())) return;

	GetCharacter()->StopJumping();
}

void ASL_MainPlayerController::SL_Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();


	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);
}

void ASL_MainPlayerController::SL_Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();	
	ProcessMouseFlick(Value.Get<FVector2D>());
	
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	if (PlayerCharacter->GetCurrentTarget()) return;
	
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ASL_MainPlayerController::SL_Primary()
{
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	
	PlayerCharacter->OnPrimaryAttack();
}

void ASL_MainPlayerController::SL_Secondary()
{
	
}

void ASL_MainPlayerController::SL_Tertiary()
{
	
}

void ASL_MainPlayerController::SL_EquipWeapon()
{
	if (ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn()); IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnAndEquipWeapon();
	}
}

void ASL_MainPlayerController::SL_TargetToggle()
{
	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	
	PlayerCharacter->FindTarget();
}


void ASL_MainPlayerController::OnASCInitialized(UAbilitySystemComponent* InASC, UAttributeSet* InAS)
{
	GetLocalPlayer()->GetSubsystem<USL_UISubsystem>()->SetAbilitySystem(InASC);
}

