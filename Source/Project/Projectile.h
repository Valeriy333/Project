// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


UCLASS()
class PROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	
protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;
	
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* TracerComponent;

public:	


};
