// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SlashHitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USlashHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARKNESSESCAPE_API ISlashHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SlashHit(FHitResult HitResult, AActor* Player, AController* PlayerController);
};
