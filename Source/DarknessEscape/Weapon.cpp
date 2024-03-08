// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Math/Vector.h"
AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	WeaponType(EWeaponType::EWT_BlackKnight)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	float RandomRotation{ FMath::FRandRange(30.f, 60.f) };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 200.f;
	UE_LOG(LogTemp, Warning, TEXT("Actor location: %s"), *GetItemMesh()->GetComponentLocation().ToString());
	GetItemMesh()->AddImpulse(ImpulseDirection);
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_PickUp);
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString WeaponTablePath{ TEXT("/Script/Engine.DataTable'/Game/_DarknessEscape/DataTables/WeaponDataTable.WeaponDataTable'") };
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_BlackKnight:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("BlackKnight"), TEXT(""));
			break;

		case EWeaponType::EWT_DragonSword:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("DragonSword"), TEXT(""));
			break;

		case EWeaponType::EWT_HeroSword:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("HeroSword"), TEXT(""));
			break;

		case EWeaponType::EWT_IceBlade:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("IceBlade"), TEXT(""));
			break;
		}

		if (WeaponDataRow)
		{
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetItemIcon(WeaponDataRow->InventoryIcon);

			Damage = WeaponDataRow->Damage;
		}
	}
}


