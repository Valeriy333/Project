// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Project/Weapon.h"
#include "Project/MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Project/MainPlayerController.h"
#include "Project/HUD/CharacterHUD.h"
#include  "Project/WeaponType.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Project/CombatState.h"
#include "Sound/SoundCue.h"
#include "Project/AnimationState.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();


		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView; 
			CurrentFOV = DefaultFOV;
		}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}


void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}
	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}



void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<AMainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ACharacterHUD>(Controller->GetHUD()) : HUD;
		 if (HUD)
		 {
			 if (EquippedWeapon)
			 {
			 	HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairCenter;
			 	HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairLeft;
			 	HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairRight;
			 	HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairBottom;
			 	HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairTop;
			 
			 }
			 else
			 {
			 	HUDPackage.CrosshairsCenter = nullptr;
			 	HUDPackage.CrosshairsLeft = nullptr;
			 	HUDPackage.CrosshairsRight = nullptr;
			 	HUDPackage.CrosshairsBottom = nullptr;
			 	HUDPackage.CrosshairsTop = nullptr;
			
			 }

		 	FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
		 	FVector2D VelocityMultiplierRange(0.f, 1.f);
		 	FVector Velocity = Character->GetVelocity();
		 	Velocity.Z = 0.f;

		 	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped( WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

		 	if (Character->GetCharacterMovement()->IsFalling())
		 	{
		 		CrosshairVelocityFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.2f, DeltaTime, 2.2f);
		 	}
		 	else
		 	{
		 		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
		 	}
		    if (bAiming)
		    {
			    CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58, DeltaTime, 30.f);
		    }
		    else
		    {
		    	CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f); 
		    }
		 	
				CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);
		 	
			
			 HUDPackage.CrosshairSpread =
			 	0.5f+
			 	CrosshairVelocityFactor +
				CrosshairInAirFactor-
					CrosshairAimFactor+
		 	CrosshairShootingFactor;	 

		 	
		 	HUD->SetHUDPackage(HUDPackage);
		 }
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon == nullptr) return;
	
	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV,
			EquippedWeapon->GetZoomFOV(),
			DeltaTime,
			EquippedWeapon->GetZoomIntepSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(
		   CurrentFOV,
		   DefaultFOV,
		   DeltaTime,
		   ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr && EquippedWeapon == nullptr) return;
	bAiming = bIsAiming;
	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
	{
		Character->ShowSniperScopeWidjet(bIsAiming);
	}
}



void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
}


void UCombatComponent::FireButtonPressed(bool bPresed)
{
	bFireButtonPressed = bPresed;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		if (Character)
		{
			Character->PlayFireMontage();
			EquippedWeapon->Fire(HitTarget);
		}
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 0.75f;
		}
		StartFireTimer();
	}
}

void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay);
}


void UCombatComponent::FireTimerFinished()
{
	if(EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	if (EquippedWeapon->IsEmpty() && CarriedAmmo > 0)
	{
		Reload();
	}
}


bool UCombatComponent::CanFire()
{
	if(EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
	 
}


void UCombatComponent::Reload()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_Reloading;
	if (CarriedAmmo > 0)
	{
		Character->PlayReloadMontage();
	}
}


void UCombatComponent::FinishReloading()
{
	if(Character == nullptr) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateValueAmmo();
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}


void UCombatComponent::UpdateValueAmmo()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<AMainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-ReloadAmount);
}



void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<AMainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}



int32 UCombatComponent::AmountToReload()
{
	if(EquippedWeapon == nullptr) return 0;
	int32 InMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(InMag, AmountCarried);
		return FMath::Clamp(InMag, 0, Least); 
	}
	return 0;
}


void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}


void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
		DropEquippedWeapon();
	
	/** віключаємо анімацію пістолета **/	
	bAnimStatePistol = false;
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	
	/** Перевіряємо який тип зброї **/
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol || EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SMG)
	{
		bAnimStatePistol = true;
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("Pistol_Socket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
		}
	}
	
	if(EquippedWeapon->GetWeaponType() != EWeaponType::EWT_Pistol || EquippedWeapon->GetWeaponType() != EWeaponType::EWT_SMG)
	{
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("Rifle_Socket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
		}
	}
	
		EquippedWeapon->SetOwner(Character);
		EquippedWeapon->SetHUDAmmo();
		InitializeCarriedAmmo();
	
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	Controller = Controller == nullptr ? Cast<AMainPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			 EquippedWeapon->EquipSound,
		Character->GetActorLocation());
	}
	
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}


void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	  
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);
	
	
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
			
		if(!TraceHitResult.bBlockingHit)
		{ 
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint; 
		}
	}
}


void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Rocket, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Sniper, StartingSniperAmmo);
}