// Fill out your copyright notice in the Description page of Project Settings.


#include "Tags/GameplayTags.h"

namespace SLTags
{
	namespace Combo
	{
		UE_DEFINE_GAMEPLAY_TAG(Opener, "SLTags.Combo.Opener");
		UE_DEFINE_GAMEPLAY_TAG(Save, "SLTags.Combo.Save");
		UE_DEFINE_GAMEPLAY_TAG(Reset, "SLTags.Combo.Reset");
	}
	
	namespace Events
	{
		namespace Tracing
		{
			UE_DEFINE_GAMEPLAY_TAG(StartTracing, "SLTags.Events.Tracing.StartTracing");
			UE_DEFINE_GAMEPLAY_TAG(StopTracing, "SLTags.Events.Tracing.StopTracing");
			UE_DEFINE_GAMEPLAY_TAG(TargetHit, "SLTags.Events.Tracing.TargetHit");
		}
		
		namespace Input
		{
			UE_DEFINE_GAMEPLAY_TAG(Primary, "SLTags.Events.Input.Primary");
			UE_DEFINE_GAMEPLAY_TAG(Dodge, "SLTags.Events.Input.Dodge");
		}
	}
	
	namespace States
	{
		UE_DEFINE_GAMEPLAY_TAG(IsAttacking, "SLTags.States.IsAttacking");
		UE_DEFINE_GAMEPLAY_TAG(IsDodging, "SLTags.States.IsDodging");
		UE_DEFINE_GAMEPLAY_TAG(MovementBlocked, "SLTags.States.MovementBlocked");
		
		namespace Attributes
		{
			UE_DEFINE_GAMEPLAY_TAG(StaminaRegenerationBlocked, "SLTags.States.Attributes.StaminaRegenerationBlocked");
		}
	}
	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG(ActivateOnGiven, "SLTags.Abilities.ActivateOnGiven");
		UE_DEFINE_GAMEPLAY_TAG(PrimaryAttack, "SLTags.Abilities.PrimaryAttack");
		UE_DEFINE_GAMEPLAY_TAG(Dodge, "SLTags.Abilities.Dodge");
		
		namespace Effects
		{
			UE_DEFINE_GAMEPLAY_TAG(Damage, "SLTags.Abilities.Effects.Damage");
		}
	}
	
	namespace Limbs
	{
		UE_DEFINE_GAMEPLAY_TAG(Torso, "SLTags.Limbs.Torso");
		UE_DEFINE_GAMEPLAY_TAG(Head, "SLTags.Limbs.Head");
		UE_DEFINE_GAMEPLAY_TAG(Center, "SLTags.Limbs.Center");
		namespace Hands
		{
			UE_DEFINE_GAMEPLAY_TAG(Left, "SLTags.Limbs.Hands.Left");
			UE_DEFINE_GAMEPLAY_TAG(Right, "SLTags.Limbs.Hands.Right");
		}
		
		namespace Legs
		{
			UE_DEFINE_GAMEPLAY_TAG(Left, "SLTags.Limbs.Legs.Left");
			UE_DEFINE_GAMEPLAY_TAG(Right, "SLTags.Limbs.Legs.Right");
		}
	}
}