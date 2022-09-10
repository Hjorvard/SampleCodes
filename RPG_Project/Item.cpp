// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set the collison volume to a created sphere component and set it to the root component
	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	//create a mesh for the object and set it to the root component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	//create a particle component and set it to the root component
	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	//Set the bool to false by default, select in blueprints for the object to rotate
	bRotate = false;
	//speed for the object to rotate at
	RotationRate = 45.f;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//create an action for if something overlaps or stops overlapping with the object
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if rotate is set to true
	if (bRotate)
	{
		//make the object rotate on the yaw axis (horizontally)
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationRate;
		SetActorRotation(Rotation);

	}

}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


