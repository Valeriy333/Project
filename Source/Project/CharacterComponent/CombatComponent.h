// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Project/Weapon.h"
#include "Project/HUD/CharacterHUD.h"
#include  "Project/WeaponType.h"
#include "Project/CombatState.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class AMainCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();
;	void Reload();
	
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	
protected:
	
	virtual void BeginPlay() override;

	void FireButtonPressed(bool bPresed);

	void Fire();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	 void SetHUDCrosshairs(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAnimStatePistol = false;

	void UpdateCarriedAmmo();

	void DropEquippedWeapon();

private:
	UPROPERTY()
	class AMainCharacter* Character;
	
	UPROPERTY()
	class AMainPlayerController* Controller;
	
	UPROPERTY()
	 class ACharacterHUD* HUD;

	UPROPERTY()
	AWeapon* EquippedWeapon;

	bool bFireButtonPressed;
	
	bool bAiming = false;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	float CrosshairVelocityFactor;

	float CrosshairInAirFactor;
	
	float CrosshairAimFactor;

	float CrosshairShootingFactor;
	

	/** Aiming and FOV **/

	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;

	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	void SetAiming(bool bIsAiming);
	
	/** Automatic fire **/
	FTimerHandle FireTimer;
	void StartFireTimer();
	void FireTimerFinished();
	
	/** Чи може стріляти **/
	bool bCanFire = true;
	bool CanFire();

	/** Поточні патрони в даний час для поточної зброї **/
	UPROPERTY()
	int32 CarriedAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 10;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 30;
	
	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 30;
	
	TMap<EWeaponType, int32> CarriedAmmoMap;
	
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 300;
	
	void InitializeCarriedAmmo();

	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	int32 AmountToReload();
	
	void UpdateValueAmmo();
	
public:
	bool ShouldSwapWeapons();
};
