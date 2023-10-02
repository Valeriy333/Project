#pragma once

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	EAS_Unarmed UMETA(DisplayName = "Unarmed "),
	EAS_Rifle UMETA(DisplayName = "Rifle"),
	EAS_Pistol UMETA(DisplayName = "Pistol"),
	
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EOnLandState : uint8
{
	EOS_NormalLand UMETA(DisplayName = "Normal Land"),
	EOS_SoftLand UMETA(DisplayName = "Soft Land"),
	EOS_HeavyLand UMETA(DisplayName = "Heavy Land"),
	
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};