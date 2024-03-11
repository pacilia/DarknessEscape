#include "Shield.h"
#include "Math/Vector.h"

AShield::AShield() :
	ThrowShieldTime(0.7f),
	bFalling(false),
	ShieldType(EShieldType::EWT_StarterShield)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AShield::ThrowShield()
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
	GetWorldTimerManager().SetTimer(ThrowShieldTimer, this, &AShield::StopFalling, ThrowShieldTime);
}

void AShield::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_PickUp);
}

void AShield::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString ShieldTablePath{ TEXT("/Script/Engine.DataTable'/Game/_DarknessEscape/DataTables/ShieldDataTable.ShieldDataTable'") };
	UDataTable* ShieldTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ShieldTablePath));

	if (ShieldTableObject)
	{
		FShieldDataTable* ShieldDataRow = nullptr;
		switch (ShieldType)
		{
		case EShieldType::EWT_StarterShield:
			ShieldDataRow = ShieldTableObject->FindRow<FShieldDataTable>(FName("StarterShield"), TEXT(""));
			break;

		case EShieldType::EWT_AdvancedShield:
			ShieldDataRow = ShieldTableObject->FindRow<FShieldDataTable>(FName("AdvancedShield"), TEXT(""));
			break;
		}

		if (ShieldDataRow)
		{
			SetPickupSound(ShieldDataRow->PickupSound);
			SetEquipSound(ShieldDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(ShieldDataRow->ItemMesh);
			SetItemName(ShieldDataRow->ItemName);
			SetItemIcon(ShieldDataRow->InventoryIcon);

			BlockSound = ShieldDataRow->BlockSound;
			Defense = ShieldDataRow->Defense;
		}
	}
}