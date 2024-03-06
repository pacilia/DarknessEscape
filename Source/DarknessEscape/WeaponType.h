#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Sword UMETA(DisplayName = "Sword"),
	EWT_Mace UMETA(DisplayName = "Mace"),
	EWT_Dagger UMETA(DisplayName = "Dagger"),
	EWT_Axe UMETA(DisplayName = "Axe"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};