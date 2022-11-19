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
	UCapsuleComponent* MyCapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	float fTotalSizeRescalingTime;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FVector FSmallSize;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void Shrink(bool shrink);
	
	/* Gets the the residual shrinked time*/
	float GetResidualShrinkTime();
	/* gets the total time the player will be shrinked*/
	float GetTotalShrinkTime();
	/* set the time the player will be shrinked*/
	void SetTime(float fFullTime);

	/*Is in charge of creating the animation to go from a shrinked state to the normal state and the opposite*/
	void ScaleCharacter(float DeltaTime);

	/* Change the time remaining for the shrink state and if it's done call the server to rescale the character*/
	void DecreaseShrinkTimeAndAct(float DeltaTime);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 
	FVector StartScale;
	float fTotalShrinkTime;
	float fResidualShrinkTime;
	float fSizeRescalingTime;
	bool bIsShrinked;
	bool bIsSizeRescaling;
};
