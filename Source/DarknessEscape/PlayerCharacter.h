// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;
class AShield;
class USoundCue;
class UParticleSystem;
class USoundCue;
class UParticleSystem;

UENUM()
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_LightAttacking UMETA(DisplayName = "LightAttacking"),
	ECS_HeavyAttacking UMETA(DisplayName = "HeavyAttacking"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_Rolling UMETA(DisplayName = "Rolling"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_Blocking UMETA(DisplayName = "Blocking"),
	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class DARKNESSESCAPE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* ShooterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Jump() override;

	void LightAttack();

	bool IsLightAttacking();

	void HeavyAttack();

	void SetHeavyAttackTimer();

	void SetHeavyAttackOnCooldown();

	void Roll();

	void RollButtonPressed();

	void InteractButtonPressed();

	UFUNCTION(BlueprintCallable)
	void FinishRoll();
	
	void SetRollOnCooldown();

	float RollCooldownTimeScale();

	float HeavyAttackCooldownTimeScale();

	void StartRollTimer();

	UFUNCTION(BlueprintCallable)
	void FinishLightAttack();

	UFUNCTION(BlueprintCallable)
	void FinishHeavyAttack();

	void PlayLightAttackMontage();

	void PlayHeavyAttackMontage();

	void BlockButtonPressed();

	void BlockButtonReleased();

	void Block();

	void EndBlock();

	UFUNCTION(BlueprintCallable)
	void FinishBlocking();

	bool TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation);

	void TraceForItem();

	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	void DropWeapon();

	void SwapWeapon(AWeapon* WeaponToSwap);

	AWeapon* APlayerCharacter::SpawnDefaultWeapon();

	void EquipShield(AShield* ShieldToEquip, bool bSwapping = false);

	void DropShield();

	void SwapShield(AShield* ShieldToSwap);

	AShield* APlayerCharacter::SpawnDefaultShield();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION(BlueprintCallable)
	void EndStun();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 AttackComboIndex = 0;

	UAnimInstance* AnimInstance;

	FTimerHandle HeavyAttackTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HeavyAttackCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bHeavyAttackOnCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	FTimerHandle RollTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RollCooldown;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bRollIsOnCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* BlockMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* BlockToIdleMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bShouldTraceForItems;

	int8 OverlappedItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItemLastFrame;
	
	bool bShouldPlayPickupSound;

	bool bShouldPlayEquipSound;

	FTimerHandle PickupSoundTimer;

	FTimerHandle EquipSoundTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	void ResetPickupSoundTimer();

	void ResetEquipSoundTimer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AShield> DefaultShieldClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AShield* EquippedShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USoundCue* MeleeImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;
public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	FORCEINLINE void SetCombatState(ECombatState State) { CombatState = State; }

	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload);

	void IncrementOverlappedItemCount(int8 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }

	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();

	void StartEquipSoundTimer();

	void GetPickupItem(AItem* Item);

	FORCEINLINE UParticleSystem* GetBloodParticle() const { return BloodParticle; }

	FORCEINLINE USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }

	void Stun();

	FORCEINLINE float GetStunChance() const { return StunChance; }

	FORCEINLINE AShield* GetEquippedShield() const { return EquippedShield; }
};
