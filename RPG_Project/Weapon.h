// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

//enum for the states the weapon can be in
UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	//when the weapon is on the ground as a pickup
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	//when the weapon is picked up and equipped
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	//default max value just for safety
	EWS_Max UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class VIKINGRPG_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	//weapons use a skeletal mesh rather than mesh used in item
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	//overide so it overides the parent component
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	//overide so it overides the parent component
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	//to equip the weapon to the character
	void Equip(class AMain* Char);

	//sound  effect to use when picking up and equipping a weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	//sound effect to use when swinging the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* SwingSound;

	//bool for whether or not the weapon has particle effects when equipped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticle;

	//the collision box for when swinging the weapon in combat
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision;

	//weapon state variable
	EWeaponState WeaponState;

	//set the weapon state
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	//get the current weapon state
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

	//function for when the weapon's collision box overlaps with an enemy during an attack
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	//function for when the weapon's collision box stops overlapping with an enemy during combat
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//a float for damage, this will be done to the enemy when being hit with a weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	//function to activate collision during animation blueprints
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	//function to deactivate collision during animation blueprints
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	//Get a damage type property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;
	//Pass this in as the controller when dealing damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;
	//setter for the weapon, in .cpp set this to main during the equip function
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }

protected:

	//an override for begin play
	//keep it protected
	virtual void BeginPlay() override;
};
