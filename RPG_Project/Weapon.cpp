// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapon::AWeapon()
{
	//create and set the Skeletal mesh for the weapon
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	//create and set the box collider for the weapon when used in combat
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	//set the weapon particles to off by default
	//if you want particles change this in the blueprint
	bWeaponParticle = false;

	//reference to weapon state
	//set it to pickup state by default
	WeaponState = EWeaponState::EWS_Pickup;

	//default value for weapon damage
	Damage = 25.f;

	//set to no collision by default, but change later during a function
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//set it so it only interacts with dynamic objects rather than static, so won't activate on walls etc
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//set everything to be ignored by the collision
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//and finally allow it to respond to the pawn channel during overlap
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void AWeapon::BeginPlay()
{
	//use super to call the parent version
	Super::BeginPlay();

	//add functionality to the combat collisions to call them when something overlaps with the box component
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	//add functionality to the combat collisions to call them when something stops overlapping with the box component
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	//call these again just to be 100% sure that they are set at the start
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//if the overlap is another actor
	//and if the weapon is in pickup state
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		//set Main to the name of the other actor overlapping by checking if you can cast to Main
		AMain* Main = Cast<AMain>(OtherActor);
		//if you the overlap can be cast to main then we have the right target
		if (Main)
		{
			//register as being overlapped with the player and ready to swap the weapon
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	//if the end of overlapping is another actor
	if (OtherActor)
	{
		//set Main to the name of the other actor overlapping by checking if you can cast to main
		AMain* Main = Cast<AMain>(OtherActor);
		//if the overlap can be cast to main then we have the desired target
		if (Main)
		{
			//set the active overlapping item to nothing
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	//check if the reference to player worked
	if (Char)
	{
		//get the controller and set the instigator
		SetInstigator(Char->GetController());

		//make it so that the camera ignores the weapon, to prevent camera zooming in and around if blocked by sword
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		//also prevent collision with the pawn the weapon is attached to
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		//on the off chance of simulating physics, make sure this doesn't do that
		SkeletalMesh->SetSimulatePhysics(false);

		//make sure there's a reference to the socket
		//this will allow swapping of weapons and having them consistently in the right place.
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		//if a reference to the socket is correctly found
		if (RightHandSocket)
		{
			//attach this item to the socket on the player
			RightHandSocket->AttachActor(this, Char->GetMesh());
			//make it so the weapon doesn't spin around in the player's hand
			bRotate = false;
			if (Char->GetEquippedWeapon())
			{
				//destroy the currently equipped weapon before setting the new weapon
				Char->GetEquippedWeapon()->Destroy();
			}
			//set the equipped weapon for the main controller to this weapon
			Char->SetEquippedWeapon(this);
			//set the overlapping item to nothing as we're no longer overlapping anything
			Char->SetActiveOverlappingItem(nullptr);

			//check for a sound associated with the weapon
			//if it has, play the correct sound
			if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);


		}
		//check if this weapon has particles off
		if (!bWeaponParticle)
		{
			//deactivate the particles on the weapon
			IdleParticlesComponent->Deactivate();
		}


	}
}


void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//check to see what the other actor is that overlapped with the weapon
	if (OtherActor)
	{
		//if the other actor is the enemy
		//then cast to the enemy
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		//if casting to the enemy was successful 
		if (Enemy)
		{
			//if the enemy has a particle system applied to it for taking damage
			if (Enemy->HitParticles)
			{
				//get the weapon socket to use as the location for where blood will emit during overlap
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				//if it succesfully finds the weapon socket
				if (WeaponSocket)
				{
					//create a reference to the location of the socket at the time that it collides
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);

					FVector EnemyLocation = Enemy->GetActorLocation();

					//activate the particle emitter where the sword collides with the enemy then destroy the particles after playing
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, EnemyLocation, FRotator(0.f), true);
				}
				
			}
			//if the enemy has a hit sound effect attached to it
			if (Enemy->HitSound)
			{
				//play the hit sound
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			//if the damage type is valid
			if (DamageTypeClass)
			{
				//make the enemy call it's take damage function and apply damage using information from this weapon
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
		
	}
}


void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	//set to query only so it doesn't apply anything like physics and only checks for overlap events
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	//set back to no collision
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

