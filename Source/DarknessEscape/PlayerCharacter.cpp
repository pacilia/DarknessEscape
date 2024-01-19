// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter() :
	CombatState(ECombatState::ECS_Unoccupied),
	RollCooldown(3.f),
	HeavyAttackCooldown(4.f),
	bHeavyAttackOnCooldown(false),
	bRollIsOnCooldown(false)
{
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Dont Rotate when the controller rotate, let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; //Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(ShooterMappingContext, 0);
		}
	}

	AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &APlayerCharacter::HandleOnMontageNotifyBegin);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LightAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::HeavyAttack);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RollButtonPressed);
	}

}

void APlayerCharacter::HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{
	//Decrement combo index
	AttackComboIndex--;

	if (AttackComboIndex < 0)
	{
		if (AnimInstance)
		{
			AnimInstance->Montage_Stop(0.4f, LightAttackMontage);
			FinishLightAttack();
		}
	}
}


void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	const FVector2D AxisValue = Value.Get<FVector2D>();

	AddMovementInput(ControlRot.Vector(), AxisValue.Y);

	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, AxisValue.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController()) {
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APlayerCharacter::Jump()
{
	Super::Jump();
}

void APlayerCharacter::LightAttack()
{
	if (!GetCharacterMovement()->IsFalling() && !IsLightAttacking())
	{
		CombatState = ECombatState::ECS_LightAttacking;
		PlayLightAttackMontage();
	}
	else 
	{
		AttackComboIndex = 1;
	}
}

bool APlayerCharacter::IsLightAttacking()
{
	if (AnimInstance && LightAttackMontage)
	{
		if (AnimInstance->Montage_IsPlaying(LightAttackMontage))
		{
			return true;
		}
	}
	return false;
}


void APlayerCharacter::HeavyAttack()
{
	if ((CombatState != ECombatState::ECS_Unoccupied) || GetCharacterMovement()->IsFalling()) return;
	if (bHeavyAttackOnCooldown) return;
	CombatState = ECombatState::ECS_HeavyAttacking;
	bHeavyAttackOnCooldown = true;
	PlayHeavyAttackMontage();
	SetHeavyAttackTimer();
}

void APlayerCharacter::SetHeavyAttackTimer()
{
	GetWorldTimerManager().SetTimer(
		HeavyAttackTimer,
		this, 
		&APlayerCharacter::SetHeavyAttackOnCooldown, 
		HeavyAttackCooldownTimeScale());
}

void APlayerCharacter::SetHeavyAttackOnCooldown()
{
	bHeavyAttackOnCooldown = false;
}

void APlayerCharacter::PlayLightAttackMontage()
{
	if(AnimInstance && LightAttackMontage)
	{
		AnimInstance->Montage_Play(LightAttackMontage);
	}
}

void APlayerCharacter::PlayHeavyAttackMontage()
{
	if (AnimInstance && HeavyAttackMontage)
	{
		AnimInstance->Montage_Play(HeavyAttackMontage);
	}
}

void APlayerCharacter::RollButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		Roll();
	}
}

void APlayerCharacter::FinishLightAttack()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::FinishHeavyAttack()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::Roll()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (bRollIsOnCooldown) return;
	CombatState = ECombatState::ECS_Rolling;

	bRollIsOnCooldown = true;

	if (AnimInstance && RollMontage)
	{
		AnimInstance->Montage_Play(RollMontage);
	}
	StartRollTimer();
}

void APlayerCharacter::StartRollTimer()
{
	CombatState = ECombatState::ECS_Rolling;
	GetWorldTimerManager().SetTimer(
		RollTimer, 
		this, 
		&APlayerCharacter::SetRollOnCooldown, 
		RollCooldownTimeScale());
}


void APlayerCharacter::FinishRoll()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::SetRollOnCooldown()
{
	bRollIsOnCooldown = false;
}

float APlayerCharacter::RollCooldownTimeScale()
{
	//TODO: Scale roll cooldown with dexterity
	return RollCooldown;
}

float APlayerCharacter::HeavyAttackCooldownTimeScale()
{
	//TODO: Scale heavy attack cool down with dexterity
	return HeavyAttackCooldown;
}
