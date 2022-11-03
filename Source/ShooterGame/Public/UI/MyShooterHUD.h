// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ShooterHUD.h"
#include "MyShooterHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AMyShooterHUD : public AShooterHUD
{
	GENERATED_BODY()
	
	UPROPERTY()
	FCanvasIcon JetpackFuelBarBg;

	UPROPERTY()
	FCanvasIcon JetpackFuelBar;

	/** Texture for HUD elements. */
	UPROPERTY()
	UTexture2D* HUDAssets03Texture;

public: 
	AMyShooterHUD();
private:
	/** Draws jetpack fuel bar. */
	void DrawJetpackFuel();
};
