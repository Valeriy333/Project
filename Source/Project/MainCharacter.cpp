// Fill out your copyright notice in the Description page of Project Settings.


 #include "MainCharacter.h"

#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Project/CharacterComponent/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "MainAnimInstance.h" 
#include "MainPlayerController.h"
#include "Project.h"
#include "Components/CapsuleComponent.h"
#include "UObject/FastReferenceCollector.h"

// Sets default values
AMainCharacter::AMainCharacter() :
	bCrouching(false),
	RifleHost("RifleHost_Socket"),
	RifleSocket("Rifle_Socket"),
	PistolHolst("PistolHost_Socket")
	
	
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));


	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh); 
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	WalkSpeed = 280.f;
	RunSpeed = 500.f;
	CrouchSpeed = 200.f;

}

 
// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHUDHealth();
}


void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideCameraIfCharacterClose();
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::PresedCrouch);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMainCharacter::EquipButtonPresed);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMainCharacter::FireButonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AMainCharacter::FireButonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AMainCharacter::AimingStart);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &AMainCharacter::AimingStop);
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}


void AMainCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0};

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0};

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::PresedCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}


void AMainCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true );
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}


void AMainCharacter::AimingStart()
{
	
	if (CharacterPlayerController == nullptr || Combat->EquippedWeapon == nullptr) return;

	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

 
void AMainCharacter::AimingStop()
{
	if (CharacterPlayerController == nullptr || Combat->EquippedWeapon == nullptr) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}


void AMainCharacter::ReloadButtonPressed()
{
	if (Combat)
	{
		Combat->Reload();
	}
}


/** Взяти зброю **/
void AMainCharacter::EquipButtonPresed()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}


void AMainCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{ 
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}


bool AMainCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}


void AMainCharacter::FireButonPressed()
{
	if (bisSprinting)
	{
		bIfRuning = true;
		bisSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}


void AMainCharacter::FireButonReleased()
{
	
	if (Combat)
	{
		if (bIfRuning)
		{
			bisSprinting = true;
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			bIfRuning = false;   
		}
		
		Combat->FireButtonPressed(false); 
	}
}


void AMainCharacter::PlayFireMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr ) return;
	 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = ("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


void AMainCharacter::PlayReloadMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr ) return;
	 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("RifleReload");
			break;
		case EWeaponType::EWT_Rocket:
			SectionName = FName("RifleReload");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("RifleReload");
			break;
		case EWeaponType::EWT_SMG:
			SectionName = FName("RifleReload");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("RifleReload");
			break;
		case EWeaponType::EWT_Sniper :
			SectionName = FName("RifleReload");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


void AMainCharacter::PlayHitReactMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr ) return;
	 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("Front");;
		AnimInstance->Montage_JumpToSection(SectionName);
	} 
}


void AMainCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
}


void AMainCharacter::UpdateHUDHealth()
{
	CharacterPlayerController = CharacterPlayerController == nullptr ? Cast<AMainPlayerController>(Controller) : CharacterPlayerController;
	if (CharacterPlayerController)
	{
		CharacterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}


ECombatState AMainCharacter::GetCombatState() const
{
	if (Combat == nullptr) return  ECombatState::ECS_MAX;
	return Combat->CombatState;
}
