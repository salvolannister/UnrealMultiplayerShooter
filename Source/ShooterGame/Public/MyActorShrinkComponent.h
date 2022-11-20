// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "My_ShooterCharacter.h"
#include "MyActorShrinkComponent.generated.h"

class UCapsuleComponent;
class AMy_ShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UMyActorShrinkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyActorShrinkComponent();
	UPROPERTY(EditDefaultsOnly, Category = Character)
	AMy_ShooterCharacter* MyCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	float fTotalSizeRescalingTime;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FVector FSmallSize;

	float const fSKIP_INTERPOLATION = 1;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void SetValueToStartScaling(bool HasToShrink, bool HasToSkipShrinkInterpolation);	
	/* Gets the the residual shrinked time*/
	float GetResidualShrinkTime();
	/* gets the total time the player will be shrinked*/
	float GetTotalShrinkTime();
	/* sets the time the player will be shrinked*/
	void SetTotalShrinkTime(float fFullTime);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 
	FVector FStartScale;
	float fStartBaseHeight;
	float fShrinkedBaseHeight;
	float fTotalShrinkTime;
	float fResidualShrinkTime;
	float fSizeRescalingTime;
	bool bIsShrinked;
	bool bIsSizeRescaling;

	/* Checks if the character is shrinked and then based on the lerping parameters scales the character to a value between the current scale and the target one */
	void SetCharacterScaleWithLerping(float fLerpingParameter);

	/* Change the time remaining for the shrink state and if it's done call the server to rescale the character*/
	void DecreaseShrinkTimeAndAct(float DeltaTime);
};
