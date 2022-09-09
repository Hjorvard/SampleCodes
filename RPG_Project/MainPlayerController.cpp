// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//if we can get a reference to the HUD Widget
	if (HUDOverlayAsset)
	{
		//set the reference to the correct blueprint widget
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

	}
	//add the hud to the viewport then set it to be visible
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	//check for the enemy health bar widget
	if (WEnemyHealthBar)
	{
		//create a reference and set it
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		//if we get the reference
		if (EnemyHealthBar)
		{
			//add to viewport but keep it hidden until called to be seen
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		//set the location to where the enemy is
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	//check for the pause menu widget
	if (WPauseMenu)
	{
		//create a reference and then set it
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		//if we find the reference
		if (PauseMenu)
		{
			//add to viewport but keep hidden until needed
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	//if we have can get the reference
	if (EnemyHealthBar)
	{
		//set the bool to true and make it visible
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthbar()
{
	//if we get the reference
	if (EnemyHealthBar)
	{
		//set the bool to false and hide the health bar widget
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//check for a reference
	if (EnemyHealthBar)
	{
		//create a reference to a location
		FVector2D PositionInViewport;
		
		//this edit the reference to a location to hopefully make it centred and above the enemy
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 75.f;
		PositionInViewport.X -= 25.f;

		//set a reference to the size of the health bar
		FVector2D SizeInViewport = FVector2D(200.f, 25.f);

		//now actually set the location and size of the health bar on the screen
		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	//set the bool to true for both pause and show the mouse cursor
	bPauseMenuVisible = true;
	bShowMouseCursor = true;
	//set the pause menu to visible (Will also play an animation from blueprints)
	PauseMenu->SetVisibility(ESlateVisibility::Visible);

	//create a reference to the input mode and set it to be valid in UI and in Game
	FInputModeGameAndUI InputModeGameAndUI;
	SetInputMode(InputModeGameAndUI);
	//pause the game world
	UGameplayStatics::SetGamePaused(GetWorld(), true);

}


void AMainPlayerController::RemovePauseMenu_Implementation()
{
	//set the bools to false and hide the menu
	bPauseMenuVisible = false;
	bShowMouseCursor = false;
	PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	//call the game mode only function
	GameModeOnly();
	//unpause the world
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AMainPlayerController::TogglePauseMenu()
{
	//if the pause menu is visible
	if (bPauseMenuVisible)
	{
		//hide the pause menu
		RemovePauseMenu();
	}
	else
	{
		//show the pause menu
		DisplayPauseMenu();
	}
}

void AMainPlayerController::GameModeOnly()
{
	//set to only interact with the game mode rather than game and UI
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}





