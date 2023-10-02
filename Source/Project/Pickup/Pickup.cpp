// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"

// Sets default values
APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	OverlapeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapeSphere->SetupAttachment(RootComponent);
	OverlapeSphere->SetSphereRadius(100.f);
	OverlapeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapeSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapeSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OickupMesh"));
	PickupMesh->SetupAttachment(OverlapeSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  
}


void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapeSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);
}


void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	 
}


void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupMesh)
	{
		PickupMesh->AddLocalRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}

}

void APickup::Destroyed()
{
	Super::Destroyed();
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation());
	}
}
