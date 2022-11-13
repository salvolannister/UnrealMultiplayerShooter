// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "My_ShooterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MyActorShrinkComponent.generated.h"



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



	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 
	FVector StartScale;
};
