// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"


UENUM(BlueprintType)
//enumerator for the assorted variations of movement, such as walk, sprint, crouch
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")

};

UENUM(BlueprintType)
//enumarator for use alongside stamina and stamina affected mechanics
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class VIKINGRPG_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	//the various states of movement the player can have
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;
	//set the movement status and movement speed of the player accordingly
	void SetMovementStatus(EMovementStatus Status);
	//create floats for walking and running
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;
	//bool for whether the player is running or not
	bool bShiftKeyDown;
	//pressed to enable sprinting
	void ShiftKeyDown();
	//release to disable sprinting
	void ShiftKeyUp();


	//Stamina related info goes here
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;
	//set the stamina status
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }
	//the drain rate of stamina when using stamina draining functions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	//the minimum amount of stamina required to start sprinting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;


	/** Camera boom to automatically position the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**The camera that follows the player around */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//the base turn rate for the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
	//the sensitivity of the turning of the camera
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float HorizontalSensitivity;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float VerticalSensitivity;


	//
	//
	//
	//Player stats go here
	//
	//
	//

	//floats to denote max health and the current health of the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	//function for losing health
	void DecrementHealth(float Amount);
	//function for losing all health and dying
	void Die();
	//the built in take damage float
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	//floats to denote the max stamina and the current stamina of the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	//float for currently collected coins
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;
	//function for adding a coin after collecting
	void IncrementCoins(int32 Amount);


	//
	//
	//
	//Weapon and combat related things go here
	//
	//
	//

	//create an effect to play when the player takes damage, set the effect in the player blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	//sound effect for when the player is hurt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	//float for how quickly the player character with interperlate to the enemy in combat
	float InterpSpeed;
	//bool for when the player is interperlating to the enemy
	bool bInterpToEnemy;
	//function to call to interp to enemy
	void SetInterpToEnemy(bool Interp);

	//get a reference to the enemy you are currently fighting to lock onto and target
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	//create an F Rotator Function to use to interp the enemy in the Yaw directions when in combat with the enemy
	//take in the vector of the enemy
	FRotator GetLookAtRotationYaw(FVector Target);


	//
	//
	//
	// Item and object Interaction goes here
	//
	//
	//

	//what to do when the pick up binding is pressed
	void PickupDown();
	//what to do when the pick up binding is released
	void PickupUp();
	//create a bool for if the button is held down
	bool bPickupDown;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//called for forward and backwards movement based on input
	void MoveForward(float Value);

	//called for left and right movement based on input
	void MoveRight(float Value);

	//called from input to turn on the horizontal / x axis at set rate
	//@param Rate is a normalised rate at 100% of desired turn rate
	void TurnAtRate(float Rate);

	//called from input to look up and down on the vertical / Y axis at a set rate
	//@Param Rate is a normalised rate at 100% of desired look up/down rate
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//
	//
	//
	//Weapon and combat related things go here
	//
	//
	//

	//get a reference to the equipped weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	//set the equipped weapon
	FORCEINLINE void SetEquippedWeapon(AWeapon* WeaponToSet) { EquippedWeapon = WeaponToSet; }
	//get the equipped weapon
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	//variable for the active overlapping item, for now just weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	//set the active overlapping item
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	//bool for when the attack button is pressed
	bool bLMB;
	//function for when the attack button is pressed
	void LMBDown();
	//function for when the attack button is released
	void LMBUp();
	//function for attacking
	void Attack();
	//function to stop attacking
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//bool for attacking, used for the animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	//animation montage class for the attack animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	//int to reference the place in the attack combo
	int ComboAttack;

	


};
