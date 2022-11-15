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
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void Shrink(bool shrink);
	
	/* Gets the the residual shrinked time*/
	float GetResidualTime();
	/* gets the total time the player will be shrinked*/
	float GetTotalTime();
	/* set the time the player will be shrinked*/
	void SetTime(float fFullTime);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 
	FVector StartScale;
	float fTotalShrinkTime;
	float fResidualShrinkTime;
	bool bIsShrinked;
};
