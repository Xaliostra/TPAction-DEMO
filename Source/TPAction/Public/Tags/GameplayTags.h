// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace SLTags
{
	namespace Combo
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Opener);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Save);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Reset);
	}
	
	namespace Events
	{
		namespace Tracing
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(StartTracing);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(StopTracing);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(TargetHit);
		}
		
		namespace Input
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dodge);
		}
	}
	
	namespace States
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(IsAttacking);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(IsDodging);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementBlocked);
		
		namespace Attributes
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(StaminaRegenerationBlocked);
		}
	}
	
	namespace Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(PrimaryAttack);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dodge);
		
		namespace Effects
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
		}
	}
	
	namespace Limbs
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Torso);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Head);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Center);
		namespace Hands
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Left);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Right);
		}
		
		namespace Legs
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Left);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Right);
		}
	}
}

