// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/UI/SL_AttributeListener.h"

#include "AbilitySystemComponent.h"

USL_AttributeListener* USL_AttributeListener::ListenForAttributeChange(UObject* WorldContextObject, UAbilitySystemComponent* ASC, FGameplayAttribute Attribute, FGameplayAttribute MaxAttribute)
{
	if (!ASC) return nullptr;
	
	USL_AttributeListener* Action = NewObject<USL_AttributeListener>();
	Action->TargetASC = ASC;
	Action->CurrentAttribute = Attribute;
	Action->CurrentMaxAttribute = MaxAttribute;
	
	Action->AttributeHandle = ASC->GetGameplayAttributeValueChangeDelegate(Attribute)
		.AddUObject(Action, &USL_AttributeListener::OnAttributeChangedCallback);
	
	if (MaxAttribute.IsValid())
	{
		Action->MaxAttributeHandle = ASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute)
			.AddUObject(Action, &USL_AttributeListener::OnMaxAttributeChangedCallback);
	}
	
	bool bFound = false;
	
	return Action;
}

void USL_AttributeListener::Shutdown()
{
	if (IsValid(TargetASC))
	{
		TargetASC->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).Remove(AttributeHandle);
		if (CurrentMaxAttribute.IsValid())
		{
			TargetASC->GetGameplayAttributeValueChangeDelegate(CurrentMaxAttribute).Remove(MaxAttributeHandle);
		}
	}
}

void USL_AttributeListener::OnAttributeChangedCallback(const FOnAttributeChangeData& Data)
{
	if (IsValid(TargetASC))
	{
		bool bFound = false;
		float MaxValue = CurrentMaxAttribute.IsValid() ? TargetASC->GetGameplayAttributeValue(CurrentMaxAttribute, bFound) : 0.0f;
		
		OnAttributeChanged.Broadcast(Data.NewValue, Data.OldValue, MaxValue);
	}
}

void USL_AttributeListener::OnMaxAttributeChangedCallback(const FOnAttributeChangeData& Data)
{
	if (IsValid(TargetASC))
	{
		bool bFound = false;
		float CurrentValue = TargetASC->GetGameplayAttributeValue(CurrentAttribute, bFound);
		
		OnAttributeChanged.Broadcast(CurrentValue, CurrentValue, Data.NewValue);
	}
}
