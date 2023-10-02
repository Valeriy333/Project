 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationState.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Project/Interfaces/InteractWithCrosshairsInterface.h"
#include "Project/CombatState.h"
#include "MainCharacter.generated.h"


UCLASS()
class PROJECT_API AMainCharacter : public ACharacter, public  IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();
	
	virtual void PostInitializeComponents() override;
	void PlayFireMontage();
	void PlayReloadMontage();
	void PlayHitReactMontage();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidjet(bool bShowScope);
protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void PresedCrouch();
	void EquipButtonPresed();
	void FireButonPressed();
	void FireButonReleased();
	void AimingStart();
	void AimingStop();
	void ReloadButtonPressed();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		class AController* InstigatorController,
		AActor* DamageCauser);

	void UpdateHUDHealth();
 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;

	/** записуєм чи біг **/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIfRuning = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EAnimationState EAnimationState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EOnLandState ELandState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bisSprinting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RifleHost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RifleSocket;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName PistolHolst;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bWeaponHand;

	
	UPROPERTY()
	class AWeapon* OverlappingWeapon;
	
	/** Componet includ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	/** Монтажі **/
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;
 
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;
	
 
	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	/** Player health **/
	UPROPERTY(EditAnywhere, Category = "Plaeyr Stats")
	float MaxHealth = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Plaeyr Stats")
	float Health = 100.f;

	UPROPERTY()
	class AMainPlayerController* CharacterPlayerController;

public:
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	
	 void SetOverlappingWeapon(AWeapon* Weapon);
	
	UFUNCTION(BlueprintCallable)
	bool IsWeaponEquipped();
	
	ECombatState GetCombatState() const;
	
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
 };
