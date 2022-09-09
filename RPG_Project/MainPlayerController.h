// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VIKINGRPG_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	//reference to the UMG Asset in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	//variable to hold the widget after creation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	//widget for enemy health bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	//widget for the pause menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	//bool for if the health bar for the enemy is visible
	bool bEnemyHealthBarVisible;
	//functions to either hide or make the enemy health bar visible
	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthbar();

	//bool for the pause menu
	bool bPauseMenuVisible;
	//functions for the pause menu
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();
	void TogglePauseMenu();
	void GameModeOnly();

	//vector for the enemy location
	FVector EnemyLocation;


protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
