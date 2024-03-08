// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class APlayerCharacter;
class UCurveFloat;
class USoundCue;
class UCurveVector;
class UDataTable;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_PickUp UMETA(DisplayName = "PickUp"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	EIS_MAX UMETA(DisplayName = "DefaultMAX")

};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Shield UMETA(DisplayName = "Shield"),
	EIT_Potion UMETA(DisplayName = "Potion"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class DARKNESSESCAPE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SetItemProperties(EItemState State);


	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlayPickupSound();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	APlayerCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;
public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }

	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

	FORCEINLINE EItemState GetItemState() const { return ItemState; }

	void SetItemState(EItemState State);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }

	FORCEINLINE void SetPickupSound(USoundCue* Sound) { PickupSound = Sound; }

	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	FORCEINLINE void SetEquipSound(USoundCue* Sound) { EquipSound = Sound; }

	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	FORCEINLINE void SetCharacter(APlayerCharacter* Char) { Character = Char; }

	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }

	FORCEINLINE void SetItemIcon(UTexture2D* Icon) { IconItem = Icon; }

	void PlayEquipSound();

	void SwapEquippedItemLocation(FVector Location);
};
