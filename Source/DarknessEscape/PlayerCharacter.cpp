// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "Item.h"
#include "Shield.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "EnemyController.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DarknessEscape.h"

APlayerCharacter::APlayerCharacter() :
	CombatState(ECombatState::ECS_Unoccupied),
	RollCooldown(3.f),
	HeavyAttackCooldown(4.f),
	bHeavyAttackOnCooldown(false),
	bRollIsOnCooldown(false),
	bShouldTraceForItems(false),
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f),
	StunChance(.25f),
	Health(300.f),
	MaxHealth(300.f),
	bDead(false)
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

	EquipWeapon(SpawnDefaultWeapon());
	EquippedWeapon->SetCharacter(this);

	EquipShield(SpawnDefaultShield());
	EquippedShield->SetCharacter(this);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForItem();
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
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BlockButtonPressed);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &APlayerCharacter::BlockButtonReleased);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractButtonPressed);
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
	if (CombatState != ECombatState::ECS_Unoccupied) return;
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

void APlayerCharacter::BlockButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		Block();
	}
}

void APlayerCharacter::BlockButtonReleased()
{
	if (CombatState != ECombatState::ECS_Blocking) return;

	EndBlock();
}

void APlayerCharacter::Block()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	CombatState = ECombatState::ECS_Blocking;

	if (AnimInstance && RollMontage)
	{ 
		AnimInstance->Montage_Play(BlockMontage);
		AnimInstance->Montage_JumpToSection(FName("Blocking"));
	}
}

void APlayerCharacter::EndBlock()
{
	if (AnimInstance && RollMontage)
	{
		AnimInstance->Montage_Play(BlockToIdleMontage);
	}
}

void APlayerCharacter::FinishBlocking()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::InteractButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(TraceHitItem);
		AShield* Shield = Cast<AShield>(TraceHitItem);
		if (Weapon)
		{
			EquippedWeapon->SwapEquippedItemLocation(TraceHitItem->GetActorLocation());
		}
		else if (Shield)
		{
			EquippedShield->SwapEquippedItemLocation(TraceHitItem->GetActorLocation());
			
		}
		else {
			//Other item
		}
		GetPickupItem(TraceHitItem);
		TraceHitItem = nullptr;
	}

}

bool APlayerCharacter::TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	//Get viewport size 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		//Trace from crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void APlayerCharacter::TraceForItem()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshair(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}

			//We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//We are hitting a difference AItem this frame from last frame or AItem is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			//Store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		//No longer overlapping any items, 
		//Item last frame should not show widget
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

void APlayerCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		EquippedWeapon = WeaponToEquip;

		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void APlayerCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void APlayerCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void APlayerCharacter::GetPickupItem(AItem* Item)
{

	if (Item)
	{
		Item->SetCharacter(this);
		Item->PlayPickupSound();
	}


	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}

	auto Shield = Cast<AShield>(Item);
	if (Shield)
	{
		SwapShield(Shield);
	}
}


AWeapon* APlayerCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	return nullptr;
}


void APlayerCharacter::EquipShield(AShield* ShieldToEquip, bool bSwapping)
{
	if (ShieldToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("LeftHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(ShieldToEquip, GetMesh());
		}

		EquippedShield = ShieldToEquip;

		EquippedShield->SetItemState(EItemState::EIS_Equipped);
	}
}


void APlayerCharacter::DropShield()
{
	if (EquippedShield)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedShield->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedShield->SetItemState(EItemState::EIS_Falling);
		EquippedShield->ThrowShield();
	}
}

void APlayerCharacter::SwapShield(AShield* ShieldToSwap)
{
	DropShield();
	EquipShield(ShieldToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

AShield* APlayerCharacter::SpawnDefaultShield()
{
	if (DefaultShieldClass)
	{
		return GetWorld()->SpawnActor<AShield>(DefaultShieldClass);
	}

	return nullptr;
}

EPhysicalSurface APlayerCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End{ Start + FVector(0.f, 0.f, -400.f) };
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void APlayerCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void APlayerCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}


void APlayerCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &APlayerCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void APlayerCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &APlayerCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void APlayerCharacter::Stun()
{
	if (Health <= 0.f) return;
	CombatState = ECombatState::ECS_Stunned;

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

void APlayerCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;
}


float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
		auto EnemyController = Cast<AEnemyController>(EventInstigator);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CharacterDead"), true);
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void APlayerCharacter::Die()
{
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
	bDead = true;

}

void APlayerCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}
