// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "UI/MyShooterHUD.h"
#include "Player/My_ShooterCharacterMovement.h"
#include "Player/My_ShooterCharacter.h"

AMyShooterHUD::AMyShooterHUD()
{
	JetpackFuelBar = UCanvas::MakeIcon(HUDAssets03Texture, 67, 212, 372, 50);
	JetpackFuelBarBg = UCanvas::MakeIcon(HUDAssets03Texture, 67, 162, 372, 50);
}

void AMyShooterHUD::DrawJetpackFuel()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(PlayerOwner);
	AMy_ShooterCharacter* MyCharacter = Cast<AMy_ShooterCharacter>(MyPC->GetCharacter());
	UMy_ShooterCharacterMovement* CharMov = Cast<UMy_ShooterCharacterMovement>(MyCharacter->GetMyMovementComponent());
	if (!CharMov)
		return;

	const float JetpackOffsetY = 75.0f;
	const float JetpackScaleMul = 0.75f;

	Canvas->SetDrawColor(FColor::White);
	const float JetpackFuelPosX = (Canvas->ClipX - JetpackFuelBarBg.UL * ScaleUI * JetpackScaleMul) / 2;
	const float JetpackFuelPosY = Canvas->ClipY - (Offset + JetpackFuelBarBg.VL + JetpackOffsetY) * ScaleUI * JetpackScaleMul;
	Canvas->DrawIcon(JetpackFuelBarBg, JetpackFuelPosX, JetpackFuelPosY, ScaleUI * JetpackScaleMul);

	float currentFuel = CharMov->GetJetpackResource();
	float totalFuel = CharMov->GetJetpackFullResource();
	const float FuelAmount = FMath::Min(1.0f, currentFuel / totalFuel);

	FCanvasTileItem TileItem(FVector2D(JetpackFuelPosX, JetpackFuelPosY), JetpackFuelBar.Texture->Resource,
	FVector2D(JetpackFuelBar.UL * FuelAmount * ScaleUI * JetpackScaleMul, JetpackFuelBar.VL * ScaleUI * JetpackScaleMul), FLinearColor::White);
	MakeUV(JetpackFuelBar, TileItem.UV0, TileItem.UV1, JetpackFuelBar.U, JetpackFuelBar.V, JetpackFuelBar.UL * FuelAmount, JetpackFuelBar.VL);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}