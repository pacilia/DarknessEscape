// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ShieldType.h"
#include "Engine/DataTable.h"
#include "Shield.generated.h"

class USoundCue;
class UWidgetComponent;
class UParticleSystem;

USTRUCT(BlueprintType)
struct FShieldDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* BlockSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense;
};


UCLASS()
class DARKNESSESCAPE_API AShield : public AItem
{
	GENERATED_BODY()

public:
	AShield();

	virtual void Tick(float DeltaTime) override;

protected:
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	FTimerHandle ThrowShieldTimer;

	float ThrowShieldTime;

	bool bFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties", meta = (AllowPrivateAccess = "true"))
	EShieldType ShieldType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ShieldDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	USoundCue* BlockSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield Properties", meta = (AllowPrivateAccess = "true"))
	float Defense;
public:
	void ThrowShield();

	FORCEINLINE EShieldType GetShieldType() const { return ShieldType; }

	FORCEINLINE USoundCue* GetBlockSound() const { return BlockSound; }

	FORCEINLINE float GetDefense() const { return Defense; }
};
