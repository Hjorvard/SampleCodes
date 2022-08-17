// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "TimerManager.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//actually create the sphere around the enemy mesh to create the agro range
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	//set and attach it to the root component, in this case the enemy
	AgroSphere->SetupAttachment(GetRootComponent());
	//set the radius of the agro sphere
	AgroSphere->InitSphereRadius(750.0f);

	//and create the sphere around the enemy mesh to detect if it is in range to attack the player
	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	//again attach to the root component.
	CombatSphere->SetupAttachment(GetRootComponent());
	//and set the radius for the attacking sphere
	CombatSphere->InitSphereRadius(95.0f);

	//create the chase spheres around the enemy mesh to decide how far to chase the player after aggroed
	ChaseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseSphere"));
	//set and attach it to the root object
	ChaseSphere->SetupAttachment(GetRootComponent());
	//set the radius for the chase sphere, must be bigger than aggro range
	ChaseSphere->InitSphereRadius(1555.5f);

	//set the combat sphere overlap to false by default
	bOverlappingCombatSphere = false;

	//the max health of the enemy
	MaxHealth = 100.f;
	//current health of the enemy
	//just set it to the same as Max Health initially
	Health = 100.f;

	//the damage the enemy deals when it attacks
	Damage = 10.f;

	//set the min time between attacks
	AttackTimeMin = 0.5f;
	//and the max time between attacks
	AttackTimeMax = 2.5f;

	//box collider for when the enemy attacks
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	//attach to the custom socket on the enemy
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));
	//set the default state of the attacking bool to false
	bAttacking = false;

	//set to no collision by default, but change later during a function
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//set it so it only interacts with dynamic objects rather than static, so won't activate on walls etc
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//set everything to be ignored by the collision
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//and finally allow it to respond to the pawn channel during overlap
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//make sure we get the AIController sorted as soon as the enemy comes into existence
	//cast to the Actor AI controller, AI Controller is derived from Controller, so we have to get that too
	//set it all to a reference called AIController, and hopefully not get myself confused
	AIController = Cast<AAIController>(GetController());

	//bind an overlap event to the components used in overlap begin and end
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	//bind an overlap event for the combat sphere, the box for when you're in range of enemy attacks
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	//bind overlap events for the chase sphere
	ChaseSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::ChaseSphereOnOverlapBegin);
	ChaseSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::ChaseSphereOnOverlapEnd);

	//set an event for the combat collision event
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	//getting an error suddenly after everything was working fine, maybe try setting the default enemy movement status here
	//didn't fix it, but something else must have, keep this here anyway just for safety I suppose
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

	//and again in begin play for for that bit of extra security
	//set to no collision by default, but change later during a function
	//CombatCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//set it so it only interacts with dynamic objects rather than static, so won't activate on walls etc
	//CombatCollision2->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//set everything to be ignored by the collision
	//CombatCollision2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//and finally allow it to respond to the pawn channel during overlap
	//CombatCollision2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//the function for when the player enters the agro range of the enemy
void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//if something is overlapping check to make sure it's valid
	if (OtherActor)
	{
		//cast to Main
		AMain* Main = Cast<AMain>(OtherActor);
		//if main is then valid
		if (Main)
		{
			//move to target and set the target to main
			MoveToTarget(Main);
		}
	}
}

//the function for when the player leaves the agro range of the enemy
void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

//the function for when the player is in the attacking range of the enemy
void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//if something is overlapping check to make sure it's valid
	if (OtherActor)
	{
		//cast to main
		AMain* Main = Cast<AMain>(OtherActor);
		//if main is then valid
		if (Main)
		{
			//set the players combat target to this enemy
			Main->SetCombatTarget(this);
			//set the combat target to the player
			CombatTarget = Main;
			//start by changing the overlap bool to true
			bOverlappingCombatSphere = true;
			//set the enemy movement status to attacking
			//this should cause the attacking animation to play 
			//the enemy should also stop chasing until after the attack animation is finished
			//replace this but tabbing out for future reference
			//replaced with the attack function, which contains this
			//SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

			//call the attack function
			Attack();
		}
	}
}

//the function for when the player leaves the attacking range of the enemy
void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if something is about to stop overlapping
	if (OtherActor)
	{
		//cast to main
		AMain* Main = Cast<AMain>(OtherActor);
		//if Main is now valid
		if (Main)
		{
			//so that if the player is still in combat with other enemies, we don't reset the combat target to nothing
			if (Main->CombatTarget == this)
			{
				//set the player target to nothing
				Main->SetCombatTarget(nullptr);
			}

			


			//set the combat overlap bool to false
			bOverlappingCombatSphere = false;

			/*
			//set the enemy movement status to follow the player again ready for another attack
			//currently tabbing this out to try and call it from the animation blueprint
			//maybe then can do full animations before cancelling them for other things
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
			MoveToTarget(Main);
			*/

			//if the enemy movement status is not equal to attacking
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{
				//then call move to target
				MoveToTarget(Main);
				//set combat target to null
				CombatTarget = nullptr;
				
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

//function for when the player overlaps with the chase sphere
//I don't think I'm actually going to ever use this, but just in case I do need it some time, I will create it and leave it empty
void AEnemy::ChaseSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

//for when the player exits the range of the chase sphere
void AEnemy::ChaseSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if something is about to stop overlapping
	if (OtherActor)
	{
		//cast to main
		AMain* Main = Cast<AMain>(OtherActor);
		//if main is now valid
		if (Main)
		{
			//set the enemy movement status to idle and stop it chasing the player who is now too far away
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		}
	}
}

//function for handling the enemy moving towards the player when in the aggro range
void AEnemy::MoveToTarget(AMain* Target)
{
	//start by changing the status of the enemy to the moving status
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	//check to make sure that the controller is set and valid
	if (AIController)
	{
		//for now just note it in the log to make sure it's working
		//UE_LOG(LogTemp, Warning, TEXT("MOVETOTARGET()"));

		//Blueprints honestly make this so much easier, but important to learn how to do it myself with code
		//deceptively simple looking and I think I could probably make it look more complicated
		//but for now just gonna have it basic, can have more parameters that could add extra depth down the line I think?

		//"MoveTo" takes in 2 parameters
		//first is FAIMoveRequest which is a constant
		//Can give it multiple different variables, such as set a target or a location
		//can also add flags, pathfinding, strafing etc. worth coming back to that later
		//so need to give it a reference and a name
		FAIMoveRequest MoveRequest;
		//first set the actor to move towards, in this case the player
		MoveRequest.SetGoalActor(Target);
		//set how close it has to get to the target to for it to consider it a success
		MoveRequest.SetAcceptanceRadius(35.0f);

		//the second paramter is the Navigation Path Pointer
		//can be used for multithreading and allowing more complex thought processes I think?
		//pretty much keeps things safe and tidy, dynamically allocates points
		//essentially automatic garbage collection without having to call the delete function
		//Keep it basic for now though and just give the NavPathPtr a name
		FNavPathSharedPtr NavPath;

		//Now use the AI Controller and call the Move To function
		//feed in the 2 parameters just set above and hopefully it should move to the player
		AIController->MoveTo(MoveRequest, &NavPath);

		/*
		------------------------------------------------------------------------------------------------------------
		----------------------------------------DEBUGGING STUFF HERE -----------------------------------------------
		------------------------------------------------------------------------------------------------------------

		Just doing this so I can see the pathing in action and fix things when needed
		going to have this commented out most of the time, but helps being able to just keep it in when needed
		*/

		/*TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Red, 10.0f, 2.5f);
		}
		*/
	}


}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//check to see what the other actor is that overlapped with the claw attack
	if (OtherActor)
	{
		//if the other actor is the player
		//then cast to the player
		AMain* Main = Cast<AMain>(OtherActor);
		//if casting to the player was successful 
		if (Main)
		{
			//if the player has a particle system applied to it for taking damage
			if (Main->HitParticles)
			{
				//get the claw socket to use as the location for where blood will emit during overlap
				const USkeletalMeshSocket* ClawSocket = GetMesh()->GetSocketByName("ClawSocket");
				//if it succesfully finds the claw socket
				if (ClawSocket)
				{

					//reference to the player location
					//FVector PlayerLocation = Main->GetActorLocation();

					FVector SocketLocation = ClawSocket->GetSocketLocation(GetMesh());

					//activate the particle emitter where the claw attack collides with the player then destroy the particles after playing
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
				
			}
			//if the player has a hit sound effect attached to it
			if (Main->HitSound)
			{
				//play the hit sound
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)
			{
				//call the take damage function of the player with this
				//take in the Damage float, and the Damage Type Class Set in the Blueprint
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}

	}
	
}


void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	//set to query only so it doesn't apply anything like physics and only checks for overlap events
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (SwingSound)
	{
		//play the sound cue
		UGameplayStatics::PlaySound2D(this, SwingSound);	

	}
}

void AEnemy::DeactivateCollision()
{
	//set back to no collision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	
	//check to get the AI controller
	if (AIController)
	{
		//first stop the movement of the enemy
		AIController->StopMovement();
		//set the enemy movement status to attacking
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		//check to make sure that the enemy is not already attacking
		if (!bAttacking)
		{

			//set the attacking bool to true
			bAttacking = true;
			//get the animation instance
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			//check to make sure it has the animation instance
			if (AnimInstance)
			{
				//play the attack animation from the enemy animation montage at 1.15x the normal speed
				AnimInstance->Montage_Play(CombatMontage, 1.15f);
				//to play the correct part of the montage
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
			//check for the sound cue for the attack
			/*if (SwingSound)
			{
				//play the sound cue
				UGameplayStatics::PlaySound2D(this, SwingSound);
			}*/
		}
	}

}

void AEnemy::AttackEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack End()"));
	//set the bool to attacking to false
	bAttacking = false;

	if (bOverlappingCombatSphere)
	{	
		//get a random variable between the min and max attack delays
		float AttackTime = FMath::FRandRange(AttackTimeMin, AttackTimeMax);
		//set a timer with this new variable, and use it to call the attack function
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		//then continue to attack
		UE_LOG(LogTemp, Warning, TEXT("Attack End Called()"));
	}
	
}

//Take Damage 
float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//if the health is 0 or lower
	if (Health <= 0.f)
	{
		//remove the health
		Health -= DamageAmount;
		//call the death function
		Die();
	}
	//otherwise
	else
	{
		// just remove the damage from the health
		Health -= DamageAmount;
	}
	//return the float
	return DamageAmount;
}

void AEnemy::Die()
{
	//create a reference to the anim instance
	UAnimInstance*AnimInstance = GetMesh()->GetAnimInstance();
	//if we get it and the anim instance it found correctly has the Combat Montage attached
	if (AnimInstance && CombatMontage)
	{
		//then play the combat montage
		AnimInstance->Montage_Play(CombatMontage, 10.f);
		//play the death section of the montage
		AnimInstance->Montage_JumpToSection(FName("Death"));
				
	}
	//set the enemy movement status to dead
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	//and remove collision where needed
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}
