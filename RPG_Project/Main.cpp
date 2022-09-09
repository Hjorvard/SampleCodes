// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"


// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create a boom camera for moving the camera around the player
	//and if the camera collides with a wall etc. pull it in towards the player
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; //the distance the camera is from player, consider dynamic later
	CameraBoom->bUsePawnControlRotation = true; //allows the camera to rotate based ont he controller

	//create the camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//attach the camera to the end of the spring arm / camera boom
	//use a socket at the end of the boom for positioning
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//the orientation of the controller
	FollowCamera->bUsePawnControlRotation = false; //stops the camera from rotating, leaves the arm to rotate instead
	//the default rates for turning based on input
	BaseTurnRate = 75.f;
	BaseLookUpRate = 65.f;
	//stop the player turning along with the camera rotation
	//honestly, that shit was annoying
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	//but still have the character turn to face forward when moving
	//prevent the character from sliding at weird angles
	//and lock it to x axis so it doesn't cause weird issues when jumping or falling
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 550.f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.25f;


	//create and set the size of the player's capsule collider
	GetCapsuleComponent()->SetCapsuleSize(30.f, 95.f);

	//info on the player's stats and collected items
	MaxHealth = 100.f;
	Health = 100.f;
	MaxStamina = 100.f;
	Stamina = 100.f;
	Coins = 0;

	//info and values for the player's movement
	RunningSpeed = 655.f;
	SprintingSpeed = 955.f;

	//set the movement bools to false by default
	//they will only be active as long as the player is moving
	bMovingForward = false;
	bMovingRight = false;

	//initialise enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	//initialise stamina mechanics
	StaminaDrainRate = 2.5f;
	MinSprintStamina = 15.f;

	//set the pickup button down bool to false by default
	bPickupDown = false;

	//the combo counter
	ComboAttack = 1;

	//the Interp speed
	InterpSpeed = 15.f;
	//set the interp bool to false by default
	bInterpToEnemy = false;

	//set combat target to false by default
	bHasCombatTarget = false;

	//set the pause menu to false by default
	bESCDown = false;

}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	//set the controller
	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if the player is dead, return from the tick function
	//this will stop the player from being able to do anything after dying
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//how much the stamina should be impacted over time
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	//use a switch for the various states rather than a bunch of if statements
	//the switch will take in the status of the player's stamina
	switch (StaminaStatus)
	{
		//if the stamina state is normal
	case EStaminaStatus::ESS_Normal:

		//if the bool saying the shift key / sprint button is down and there's movement
		if (bShiftKeyDown && bMovingForward == true || bShiftKeyDown && bMovingRight == true)
		{
			//if the player's stamina is less than or equal to the minimum amount of stamina needed to sprint
			//after the calculation from draining stamina
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				//set the stamina state to below minimim
				//continue to drain stamina
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			//if the player has enough stamina after draining calculations
			else
			{
				//just drain stamina
				Stamina -= DeltaStamina;
			}
			//and set the movement state to sprinting
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else // shift key released / up
		{
			//if the player's stamina is equal to or higher than max stamina after regenerating stamina
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				//then just set the stamina to max stamina
				Stamina = MaxStamina;
			}
			//however if the player's stamina is below max stamina 
			else
			{
				//continue to restore stamina to the player
				Stamina += DeltaStamina;
			}
			//the the movement state to regular rather than sprinting
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		//breakdance or everything does break!
		break;

		//if the stamina status is below the minimum required to sprint
	case EStaminaStatus::ESS_BelowMinimum:

		//if the bool saying the shift key / sprint button is down
		if (bShiftKeyDown && bMovingForward == true || bShiftKeyDown && bMovingRight == true)
		{
			//if the player's stamina is less than or equal to 0 after the calculations
			//to drain stamina from the player whilst sprinting
			if (Stamina - DeltaStamina <= 0.f)
			{
				//set the player's stamina state to echausted
				//set the stamina to 0 the minimum amount
				//and stop the player from sprinting by setting the movement state to normal
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			//if the player does still have stamina after calculations
			else
			{
				//continue to drain stamina
				//and keep the movement state as sprinting
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else // shift key released / up
		{
			//if the player's stamina is greater than or equal to the minimum amount needed to sprint
			//once the stamina regen calculation is complete
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				//set the stamina state to normal
				//continue to restore stamina
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			//if the stamina is still below the minimum amount needed to sprint
			else
			{
				//then just restore stamina
				Stamina += DeltaStamina;
			}
			//set the movement state to normal as we're not sprinting
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}

		//breakdance woooo wooooo woooo
		break;

		//if the player's stamina state is exhausted
	case EStaminaStatus::ESS_Exhausted:

		//if the player holds down the shift key
		if (bShiftKeyDown && bMovingForward == true || bShiftKeyDown && bMovingRight == true)
		{
			//keep setting the stamina to 0, nothing should happen if you don't rest
			Stamina = 0.f;
		}
		else //shift key released / up
		{
			//set the stamina state to exhausted and recovering
			//start restoring stamina to the player
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		//set the movement state to normal since the player is in no fit state to run
		SetMovementStatus(EMovementStatus::EMS_Normal);

		//Na na na na, na na, na na, Can't touch this!
		break;

		//if the player's stamina state is exhausted recovering
	case EStaminaStatus::ESS_ExhaustedRecovering:

		//if the player's stamina is greater than or equal to the minimum amount needed to sprint
		//after the calculations to restore stamina are done
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			//set the player's stamina state back to normal
			//and continue restoring stamina as normal
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		//if the player's stamina is still lower than the minimum amount needed to sprint
		//after the calculations to restore stamina have finished
		else
		{
			//continue to just restore stamina
			Stamina += DeltaStamina;
		}
		//set the movement state to normal as we're still not able to sprint
		SetMovementStatus(EMovementStatus::EMS_Normal);

		//and break again to end the case
		break;

		//just for safety 
	default:
		;
	}
	//if interp to enemy is true, and the player has a current combat target
	if (bInterpToEnemy && CombatTarget)
	{
		//get the location to look to 
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		//for smoothing purposes use this and take in the current location of the actor and direction facing
		//the direction needed to rotate to the enemy, delta time, and the float to smooth it out
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		//now set the players rotation to interp rotation
		SetActorRotation(InterpRotation);
	}

	//if the player has a combat target
	if (CombatTarget)
	{
		//set the combat target location variable to the location of the enemy each tick
		CombatTargetLocation = CombatTarget->GetActorLocation();
		//check for the player controller
		if (MainPlayerController)
		{
			//get the location of the enemy currently
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//macro to check that the player input component is valid and works
	//stop it if it is not valid
	check(PlayerInputComponent);

	//bind the axis for moving forwards and backwards
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	//bind the axis for moving left and right
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	//bind the axis for looking left and right
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//bind the axis for looking up and down
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//get input for the rate of turning vertically and horizontally
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	//get the action for jumping
	//get it for when pressed down
	//and when the player is alive
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	//get the action for jumping, to stop jumping
	//get it for when the jump button is released
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//get the action for when the player is sprinting
	//when the sprint button is pressed down
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	//the action to stop sprinting
	//when the sprint button is released
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	//get the action for picking up certain items such as weapons
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMain::PickupDown);
	//when the pickup button is released
	PlayerInputComponent->BindAction("Pickup", IE_Released, this, &AMain::PickupUp);

	//get the action for when the attack button is pressed down
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	//when the attack button is released
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	//get the action for when the pause menu button is pressed
	//PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	//get the action for when the pause menu button is released
	//PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);
	//so the player can unpause the game with the pause button
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp).bExecuteWhenPaused = true;
}

//move the player forwards and backwards
void AMain::MoveForward(float Value)
{
	//set bmoving forward to false by default since this gets checked every frame
	bMovingForward = false;

	//check to see if it's a null pointer, and that the value is not 0
	//also for now make it check to see if the player is attacking and if they are disable movement
	//possibly come back to this later to re-enable movement and improve animations
	if ((Controller != nullptr) && (Value != 0.0f) && !bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		//find out which way is forward by getting rotation and yaw
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		//get the forward vector from the rotation matrix of the rotator on ther X axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//add movement input in the direction
		AddMovementInput(Direction, Value);

		//now there's movement set bmovingforward to true
		bMovingForward = true;

	}

}

//move the player left and right
void AMain::MoveRight(float Value)
{
	//set bmovingright to false by default, again checked every second
	bMovingRight = false;

	//check to see if it's a null pointer, and that the value isn't 0
	//disable moving if attacking
	//maybe come back to that later
	if ((Controller != nullptr) && (Value != 0.0f) && !bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		//find which was is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		//get the forward vector from the rotation matrix on the y axis
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//add movement input in this direction 
		AddMovementInput(Direction, Value);

		//now there's movement again set the bmoving right to true
		bMovingRight = true;
	}
}

//the turn values for the player in regards to their position in the world
//in this case the horizontal axis
void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

//the turn values for the player in regards to their position in the world
//in this case the vertical axis
void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


//function for when the player takes damege
void AMain::DecrementHealth(float Amount)
{
	//if the player runs out of health after taking damage
	if (Health - Amount <= 0.0f)
	{
		Health -= Amount;
		Die();
	}
	//if the player still has health after taking damage
	else
	{
		Health -= Amount;
	}
}

//when the player runs out of health
void AMain::Die()
{
	//if the player is already dead, return out of this function
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	//create a reference to the anim instance
	UAnimInstance*AnimInstance = GetMesh()->GetAnimInstance();
	//if we get it and the anim instance it found correctly has the Combat Montage attached
	if (AnimInstance && CombatMontage)
	{
		//then play the combat montage
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		//play the death section of the montage
		AnimInstance->Montage_JumpToSection(FName("Death"));
		
	}

	//set the movement status to dead
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

//function for when the player collects money
void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}


//set the movement state of the player
//this effects speed primarily
void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	//if the player is sprinting
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		//set the movement speed of the player to sprinting speed
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	//if the player is not sprinting
	else
	{
		//set the character's movement speed to the maximum walking speed
		//use of a controller allows for varied speed, but keyboard and mouse
		//hase this set to a default value
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

//if the shift key is pressed down
void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

//if the shift key is released
void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

//when the player presses or holds down the pickup binding
void AMain::PickupDown()
{
	//set the bool to pickup as true
	bPickupDown = true;

	//check to see if we're overlapping with an item that can be picked up
	if (ActiveOverlappingItem)
	{
		//see if we can cast to the weapon
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		//if this works then we can move onto equipping
		if (Weapon)
		{
			//equip the weapon we're overlapped with
			Weapon->Equip(this);
			//set overlapping to nothing since the weapon is now equipped
			//have to do this because once the item is picked up it is impossible 
			//to have the overlap end since there is no more overlap there
			SetActiveOverlappingItem(nullptr);
		}
	}

}

//when the player releases the pickup binding
void AMain::PickupUp()
{
	//set the bool to false
	bPickupDown = false;
}

//when the player presses down the attack binding
void AMain::LMBDown()
{
	//set the attack button bool to true
	bLMB = true;
	
	//if the movement status is dead then return from this function
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//check to see if the player has a weapon equipped
	if (EquippedWeapon)
	{
		//call the attack function
		Attack();
	}


}

//when the player releases the attack binding
void AMain::LMBUp()
{
	//set the attack button bool to false
	bLMB = false;
}

//the attack function
void AMain::Attack()
{
	//check to see if we're already attacking and that we aren't already dead
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		//set the attacking bool to true
		bAttacking = true;
		//set the interp bool to true
		SetInterpToEnemy(true);
		
		//set the animation instance
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		//test
		if (ComboAttack == 1)
		{

			//if we get the animation instance and it's combat montage
			if (AnimInstance && CombatMontage)
			{
				//play the animation
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
				ComboAttack ++;
				//play the sound effect of the weapon swinging
				if (EquippedWeapon->SwingSound)
				{
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
				}
			}
		}
		else if (ComboAttack == 2)
		{
			//if we get the animation instance and it's combat montage
			if (AnimInstance && CombatMontage)
			{
				//play the animation
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
				ComboAttack++;
				//play the sound effect of the weapon swinging
				if (EquippedWeapon->SwingSound)
				{
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
				}
			}
		}
		else if (ComboAttack == 3)
		{
			//if we get the animation instance and it's combat montage
			if (AnimInstance && CombatMontage)
			{
				//play the animation
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack3"), CombatMontage);
				ComboAttack++;
				//play the sound effect of the weapon swinging
				if (EquippedWeapon->SwingSound)
				{
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
				}
			}
		}
		else if (ComboAttack == 4)
		{
			//if we get the animation instance and it's combat montage
			if (AnimInstance && CombatMontage)
			{
				//play the animation
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack4"), CombatMontage);
				ComboAttack++;
				//play the sound effect of the weapon swinging
				if (EquippedWeapon->SwingSound)
				{
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
				}
			}
		}
		else if (ComboAttack == 5)
		{
			//if we get the animation instance and it's combat montage
			if (AnimInstance && CombatMontage)
			{
				//play the animation
				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack5"), CombatMontage);
				ComboAttack = 1;
				//play the sound effect of the weapon swinging
				if (EquippedWeapon->SwingSound)
				{
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
				}
			}
		}
		
	}
}

void AMain::AttackEnd()
{
	//set interperlate to enemy to false so the player stops spinning around towards the enemy
	SetInterpToEnemy(false);
	//set attacking to false
	bAttacking = false;
	//if the bool saying the mouse is still pressed down
	if (bLMB)
	{
		//attack again
		Attack();
	}
	else
	{
		//otherwise reset the combo to 1.
		ComboAttack = 1;
		
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	//find the rotation needed to look at the target
	//takes in the current player location and the location of the current enemy target
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	//now create a new one and set the values to 0 on the x and y axis, but keep the information from the Z axis
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	//now return the value
	return LookAtRotationYaw;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//call the decrement health function to reduce the player's health
	DecrementHealth(DamageAmount);
	//rturn the damage amount
	return DamageAmount;
}

void AMain::DeathEnd()
{
	//pause animations at the end of the death animation and stop the skeleton from updating
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::Jump()
{
	//if the player's movement status is set to dead
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		//don't allow this to happen
		Super::Jump();
	}
}

void AMain::UpdateCombatTarget()
{
	//create an array for all the overlapping actors
	TArray<AActor*> OverlappingActors;
	//find all the overlapping actors with the player
	//filter out classes that aren't enemies
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	//check if there's no overlapping actors
	if (OverlappingActors.Num() == 0)
	{
		//check for the main player controller
		if (MainPlayerController)
		{
			//remove the enemy health bar widget by calling the function to do so
			MainPlayerController->RemoveEnemyHealthbar();
		}
		//and return from this function
		return;
	}
	//Cast to AEnemy to find overlapping actors starting with the closest(hopefully)
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	//if there is an enemy nearby
	if (ClosestEnemy)
	{
		//FVector the the location of the player
		FVector Location = GetActorLocation();
		//float for the minimum distance between the player and an enemy
		//get this by fining the magnitude between the player and the enemy
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		//create a range based for loop to cycle through the array and see which enemy is closest
		for (auto Actor : OverlappingActors)
		{
			//cast to nearby enemies
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			//and if this comes back with an enemy nearby
			if (Enemy)
			{
				//enemy's distance to the player
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();

				//if an enemy is closer than the previous min distance
				if (DistanceToActor < MinDistance)
				{
					//set Min distance to the new closest actor
					MinDistance = DistanceToActor;
					//set the new closest enemy to the enemy
					ClosestEnemy = Enemy;
				}
			}
		}
		//when the for loop has finished
		//Check to make sure the player controller is there
		if (MainPlayerController)
		{
			//call the display enemy health bar function
			MainPlayerController->DisplayEnemyHealthBar();
		}
		//set the combat target to the enemy closest to the player
		SetCombatTarget(ClosestEnemy);
		//set the enemy as a target bool to true
		bHasCombatTarget = true;

	}
}

void AMain::IncrementHealth(float Amount)
{
	//if after healing the health goes above the max amount
	if (Health + Amount >= MaxHealth)
	{
		//set health to the max amount
		Health = MaxHealth;
	}
	//if healing doesn't increase health above max health
	else
	{
		//heal by the amount set in the health potion blueprint
		Health += Amount;
	}
}

void AMain::ESCDown()
{
	//set the pause menu bool to true
	bESCDown = true;

	//if we can get to the main player controller
	if (MainPlayerController)
	{
		//call the toggle pause menu fucntion from the main player controller
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	//set the pause menu button to false
	bESCDown = false;
}






