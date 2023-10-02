// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"

#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UMainAnimInstance::UMainAnimInstance()
{
	
}


void UMainAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
}

void UMainAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MainCharacter == nullptr )
	{
		MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	}
	if (MainCharacter)
	{
		bCrouched = MainCharacter->bIsCrouched;
		
		FVector Velocity{ MainCharacter->GetVelocity() };
		
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = MainCharacter->GetCharacterMovement()->IsFalling();

		if (MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bAccelerating = true; 
		}
		else
		{
			bAccelerating = false;
		}

		bAiming = MainCharacter->GetAiming();

		if (MainCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}
}


//bWeaponEquipped = MainCharacter->IsWeaponEquipped();
 
