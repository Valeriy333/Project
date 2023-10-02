// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse,
                              const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerControler = OwnerCharacter->Controller;
		if (OwnerControler)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerControler, this, UDamageType::StaticClass());
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
