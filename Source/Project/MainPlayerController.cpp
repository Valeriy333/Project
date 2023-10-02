// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include  "HUD/CharacterHUD.h"
#include "HUD/CharacterOverlay.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CharacterHUD = Cast<ACharacterHUD>(GetHUD());
}

void AMainPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	 CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;

	bool bHUDValid = CharacterHUD &&
		CharacterHUD->CharacterOverlay &&
			CharacterHUD->CharacterOverlay->HealthBar &&
				CharacterHUD->CharacterOverlay->HealthText;
	 if (bHUDValid)
	 {
		 const float HealthPercent = Health / MaxHealth;
	 	CharacterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	 	FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	 	CharacterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	 }
}


void AMainPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	bool bHUDValid = CharacterHUD &&
		CharacterHUD->CharacterOverlay &&
			CharacterHUD->CharacterOverlay->HealthBar &&
				CharacterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		CharacterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	
}


void AMainPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	CharacterHUD = CharacterHUD == nullptr ? Cast<ACharacterHUD>(GetHUD()) : CharacterHUD;
	bool bHUDValid = CharacterHUD &&
		CharacterHUD->CharacterOverlay &&
			CharacterHUD->CharacterOverlay->HealthBar &&
				CharacterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		CharacterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}
