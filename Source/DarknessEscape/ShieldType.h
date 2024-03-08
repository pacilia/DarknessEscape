#pragma once

UENUM(BlueprintType)
enum class EShieldType : uint8
{
	EWT_StarterShield UMETA(DisplayName = "StarterShield"),
	EWT_AdvancedShield UMETA(DisplayName = "AdvancedShield"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};