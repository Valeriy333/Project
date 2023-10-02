// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Project/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector& HitTarget)
{

	AWeapon::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
		
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController )
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		uint32 Hits = 0;
		TMap<AMainCharacter*, uint32> HitMap;
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
		FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget,FireHit);
			AMainCharacter* MainCharacter = Cast<AMainCharacter>(FireHit.GetActor());
			 
			if (MainCharacter && InstigatorController)
			{
				if(HitMap.Contains(MainCharacter))
				{
					HitMap[MainCharacter]++;
				}
				else
				{
					HitMap.Emplace(MainCharacter,1);
				}
			}
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					 FireHit.ImpactNormal.Rotation());   
			}
			if (HitSound)
			{
				UGameplayStatics::SpawnSoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint);
					
			}
		}
		for (auto HitPair: HitMap)
		{
			if (InstigatorController)
			{
				if (HitPair.Key && InstigatorController)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage * HitPair.Value ,
						 InstigatorController,
						this,
						 UDamageType::StaticClass());
				}
			}
		}
	}
}
