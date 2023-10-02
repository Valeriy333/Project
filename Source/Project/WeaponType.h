#pragma once
#define TRACE_LENGTH 80000.f
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	EWT_Rocket UMETA(DisplayName = "Rocket"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_Sniper UMETA(DisplayName = "Sniper"),
	
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
	
};
