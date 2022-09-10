// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class VIKINGRPG_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	//create the box that spawns the effect and gives the area for the enemy or object to spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;

	//create references of actors, use this in blue prints to assign an actor to spawn
	//should be able to spawn both enemies, and items
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor6;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor7;
	//reference to the array of actors
	TArray<TSubclassOf<AActor>> SpawnArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//function called to find the area in the box to spawn the actor
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	//reference to the object that will be spawned
	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor> GetSpawnActor();

	//function to call to spawn an enemy, needs to be _implementation
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);

};
