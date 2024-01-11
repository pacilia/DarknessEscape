// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DEAttribiteSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \ 
#define	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

UCLASS()
class DARKNESSESCAPE_API UDEAttribiteSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UDEAttribiteSet();

	//Attribute setup for Health stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDEAttribiteSet, Health);

	//Attribute setup for Stamina stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDEAttribiteSet, Stamina);

	//Attribute setup for Strength stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UDEAttribiteSet, Strength);

	//Attribute setup for Vigor stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UDEAttribiteSet, Vigor);

	//Attribute setup for Endurance stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Endurance;
	ATTRIBUTE_ACCESSORS(UDEAttribiteSet, Endurance);

	//Attribute setup for Dexterity stat
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Dexterity;
	ATTRIBUTE_ACCESSORS(UDEAttribiteSet, Dexterity);
};
