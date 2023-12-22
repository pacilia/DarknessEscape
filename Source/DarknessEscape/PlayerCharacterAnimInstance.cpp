// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	}
	if (PlayerCharacter)
	{
		//Get the lateral speed of the character
		FVector Velocity{ PlayerCharacter->GetVelocity() };
		Velocity.Z = 0;

		Speed = Velocity.Size();

		//Check if the character in the air
		bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();

		//Check if the character is moving
		if (PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
	}
}

void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}
