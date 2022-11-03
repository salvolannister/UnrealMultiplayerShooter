// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "My_ShooterCharacter.h"
#include "My_ShooterCharacterMovement.h"

// With this constructor Unreal will automatically initialize this Character with my new Movement Component
AMy_ShooterCharacter::AMy_ShooterCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UMy_ShooterCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
}

// Input binding is set in the character in the original project, overriding this method let us expand those bindings
void AMy_ShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binding for teleport
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMy_ShooterCharacter::Teleport);

	// Binding for jetpack
	PlayerInputComponent->BindAction("UseJetpack", IE_Pressed, this, &AMy_ShooterCharacter::UseJetpack);
	// IE_Released stop jetpacking 
	PlayerInputComponent->BindAction("UseJetpack", EInputEvent::IE_Released, this, &AMy_ShooterCharacter::StopJetpacking);
}

// Function that will call the Teleport implementation in the Movement Component
void AMy_ShooterCharacter::Teleport()
{
	if (GetMyMovementComponent())
	{
		GetMyMovementComponent()->Teleport();
	}
}

void AMy_ShooterCharacter::UseJetpack() 
{
	if (GetMyMovementComponent())
	{
		Super::Jump();
		GetMyMovementComponent()->UseJetpack();
	}
}

void AMy_ShooterCharacter::StopJetpacking()
{
	if (GetMyMovementComponent())
	{
		Super::StopJumping();
		GetMyMovementComponent()->StopJetpacking();
	}
}

// Function for get our Movement Component 
UMy_ShooterCharacterMovement* AMy_ShooterCharacter::GetMyMovementComponent() const
{
	return Cast<UMy_ShooterCharacterMovement>(GetCharacterMovement());
}