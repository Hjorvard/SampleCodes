// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"
#include "Enemy.h"
#include "AIController.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create the spawnbox and reference to it
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	//if all the actors have been assigned to the spawner in its blueprint
	if (Actor1 && Actor2 && Actor3 && Actor4 && Actor5 && Actor6 && Actor7)
	{
		//add all the actors to the spawn array
		SpawnArray.Add(Actor1);
		SpawnArray.Add(Actor2);
		SpawnArray.Add(Actor3);
		SpawnArray.Add(Actor4);
		SpawnArray.Add(Actor5);
		SpawnArray.Add(Actor6);
		SpawnArray.Add(Actor7);
	}
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	//get the size of the spawn box and it's location
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();
	//get a random location within the bounds of the box
	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	//return
	return Point;

}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	//if there's something to spawn
	if (ToSpawn)
	{
		//get a reference to the world
		UWorld* World = GetWorld();
		//get a reference to the spawning parameters
		FActorSpawnParameters SpawnParams;

		//if world is valid
		if (World)
		{
			//create a reference to the actor that's going to be spawned and spawn it
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

			//if it's an enemy
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				//spawn the enemy with a contoller so it is able to move and attack etc
				Enemy->SpawnDefaultController();

				//create a reference to the controller
				AAIController* AICont = Cast<AAIController>(Enemy->GetController());
				//if it's successful
				if (AICont)
				{
					//attach the controller to the enemy
					Enemy->AIController = AICont;
				}
			}
		}
	}
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	//if the array number is greater than 0
	if (SpawnArray.Num() > 0)
	{
		//get a random number between 0 and the spawn array so 1-7
		//then subtract by 1 since 0 is counted as the first so if we got 7 there wouldn't be anything to reference and we'd crash
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);

		//return with an actor to reference when spawning
		return SpawnArray[Selection];
	}
	else
	{
		//otherwise return from this function
		return nullptr;
	}
}

