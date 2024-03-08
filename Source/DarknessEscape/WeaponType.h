#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_BlackKnight UMETA(DisplayName = "BlackKnight"),
	EWT_DragonSword UMETA(DisplayName = "DragonSword"),
	EWT_HeroSword UMETA(DisplayName = "HeroSword"),
	EWT_IceBlade UMETA(DisplayName = "IceBlade"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};