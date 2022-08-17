// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Enemy.generated.h"


//an enum class to store the the various states that the enemy can be in
//Blueprint type so can use Blueprints to tweak easier
UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
	//the idle state for the enemy, when it's just stood still doing nothing
	EMS_Idle			UMETA(DisplayName = "Idle"),
	//the state the enemy goes into when a player enters it's visibility range and moves towards the player
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	//if the enemy is close enough to the player to attack, the enemy will go into this state
	EMS_Attacking		UMETA(DisplayName = "Attacking"),

	//if the enemy is dead
	EMS_Dead			UMETA(DisplayName = "Dead"),

	//the default max state, this should never be called and is just a placeholder 
	EMS_MAX				UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class VIKINGRPG_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	//A variable to set the movement status of the enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	//set the movement status of the enemy
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status)
	{
		EnemyMovementStatus = Status;
	}

	//set and reference to a sphere around the enemy for it's agro range
	//this will let the enemy know when a player is in range to move towards the,
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	//set and reference another sphere around the enemy for when it's in range to attack
	//this will let the enemy change to an attack rather than just following the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;

	//set a reference for a sphere around the enemy to know how far the player must be away before it stops chasing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* ChaseSphere;

	//Create an AI Controller so that we can use things like "MoveTo"
	//Remember the A at the start is for Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	//create a float for the Max Health of the Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	//create a float for the current health of the enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	//create a float for the damage the enemy deals
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	//create an effect to play when the enemy takes damage, set this in the blueprint depending on the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	//sound effect for when the enemy is hit by a weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	//box collider for when the enemy attacks
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* CombatCollision;

	//sound effect for when the enemy attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* SwingSound;

	//function to activate collision during animation blueprints
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	//function to deactivate collision during animation blueprints
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	//the animation montage for combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	//bool for the attacking state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bAttacking;

	//Function to attack
	UFUNCTION(BlueprintCallable)
	void Attack();

	//Function to end the attack state
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//timer to control give the enemy a delay between attacks rather than have it just spam attacks
	FTimerHandle AttackTimer;

	//float for the minimum amount of time between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTimeMin;

	//float for the maximum amount of time between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackTimeMax;

	//for use when the enemy damages the player
	//using the basic information for this, can expand upon for more detailed damage types
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	//the built in take damage float
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//function to have the enemy die
	void Die();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//functions to be used in the CPP script for when the player overlaps the agro and combat spheres
	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//going to try adding another sphere called the chase sphere
	//hopefully if I get this to work then the enemy will chase the player for a further distance than the usual agro range
	UFUNCTION()
		virtual void ChaseSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		virtual void ChaseSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//create a function to handle moving to a target, in this case the main player, then store it as Target
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	//a bool to handle the attack animation
	//hopefully this will stop the animation ending before it's finished
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	//a reference to the main that can be used from blueprints
	//mainly used to get a quick target in blueprints or when something is attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* CombatTarget;

	//function for when the enemy's claw collision box overlaps with the player during an attack
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	//function for when the enemy's claw collision box stops overlapping with the player during combat
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
