// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "My_ShooterCharacterMovement.h"
#include "Engine/GameEngine.h"
#include "Math/UnrealMathUtility.h"
#include "ECustomMovementMode.h"
#include "My_ShooterCharacter.h"

UMy_ShooterCharacterMovement::UMy_ShooterCharacterMovement()
{
	TeleportOffset = 1000;//10m
	MaxHoldJetpackTime = 5; //5s
	JetpackForce = 15000.f;
	JetpackFullRechargeSeconds = 10;
	fJetpackFullResource = 1.0;
	fJetpackResource = 1.0;
}


bool UMy_ShooterCharacterMovement::IsShrinked() 
{
	return bIsShrinked;
}

void UMy_ShooterCharacterMovement::SetShrinkedState(uint8 bShrink, float fShrinkTime )
{
	
	ChangeScale(bShrink);

	if (IsClient()) 
	{
		Server_SetShrinkedState(bShrink, fShrinkTime);
	}

	bIsShrinked = bShrink;
}

void UMy_ShooterCharacterMovement::ChangeScale(uint8 bShrink) 
{
	AMy_ShooterCharacter* Char = Cast<AMy_ShooterCharacter>(UMy_ShooterCharacterMovement::CharacterOwner);
	if (bShrink) 
	{
		UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();
		CapsuleComponent->SetWorldScale3D(FVector((2,2,2)));
	}
	else 
{

	}
}

void UMy_ShooterCharacterMovement::Server_SetShrinkedState_Implementation(uint8 bShrink, float fShrinkTime = 0) 
{
	// change scale
	ChangeScale(bShrink);

}
bool UMy_ShooterCharacterMovement::Server_SetShrinkedState_Validate(uint8 bShrink, float fShrinkTime = 0)
{
	return true;
}


bool UMy_ShooterCharacterMovement::IsClient()
{
	return PawnOwner->GetLocalRole() == ENetRole::ROLE_AutonomousProxy;
}

// Function called from the input bindings in the character ( press J)
void UMy_ShooterCharacterMovement::UseJetpack()
{

	// Calcolate the Location to teleport to
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Wants to use jetpack");

	// If we are the client and we have control we send the location to the server 
	if (IsClient())
		Server_SetJetpackVelocity(Velocity.Z);

	// This boolean will trigger the movement in the OnMovementUpdated function native of the standard Movement Component
	bWantsToUseJetpack = true;
}

void UMy_ShooterCharacterMovement::StopJetpacking()
{

	// Calcolate the Location to teleport to
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Wants to stop using jetpack");

	// If we are the client and we have control we send the location to the server 
	if (IsClient())
		Server_SetJetpackVelocity(Velocity.Z);

	// This boolean will trigger the movement in the OnMovementUpdated function native of the standard Movement Component
	bWantsToUseJetpack = false;
}

float UMy_ShooterCharacterMovement::GetJetpackResource()
{
	return fJetpackResource;
}

float UMy_ShooterCharacterMovement::GetJetpackFullResource()
{
	return fJetpackFullResource;
}

//TODO: implement a check to avoid cheater tricks
bool UMy_ShooterCharacterMovement::Server_SetJetpackVelocity_Validate(float JetpackVelocity)
{
	return true;
}

void UMy_ShooterCharacterMovement::Server_SetJetpackVelocity_Implementation(float JetpackVelocity)
{
	Velocity.Z += JetpackVelocity;
}


#pragma region teleport
// Function called from the input binding in the character
void UMy_ShooterCharacterMovement::Teleport()
{

	TeleportLocation = PawnOwner->GetActorLocation() + PawnOwner->GetActorForwardVector() * TeleportOffset;

	// If we are the client and we have control we send the location to the server 
	if (PawnOwner->GetLocalRole() == ENetRole::ROLE_AutonomousProxy) {

		// Calcolate the Location to teleport to
		Server_SendTeleportLocation(TeleportLocation);
	}

	// This boolean will trigger the movement in the OnMovementUpdated function native of the standard Movement Component
	bWantsToTeleport = true;
}

// Here we can validate the location of the teleport to prevent cheating
bool UMy_ShooterCharacterMovement::Server_SendTeleportLocation_Validate(FVector LocationToTeleport)
{
	return true;
}

// Here we save the location sent in the local variable
void UMy_ShooterCharacterMovement::Server_SendTeleportLocation_Implementation(FVector LocationToTeleport)
{
	TeleportLocation = LocationToTeleport;
}

#pragma endregion 



bool UMy_ShooterCharacterMovement::CanUseJetpack()
{
	//Disable jetpacking if fuel is empty
	if (fJetpackResource <= 0)
	{
		return false;
	}

	return true;
}

//Is run on both server and clien
void UMy_ShooterCharacterMovement::PhysJetpack(float deltaTime, int32 Iterations)
{


	if (!bWantsToUseJetpack ||
		
		fJetpackResource <= (deltaTime / MaxHoldJetpackTime))
	{
		bWantsToUseJetpack = false;
		SetMovementMode(EMovementMode::MOVE_Falling);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	float resultingAccel = JetpackForce / Mass;
	float jetpackSurplusAccel = FMath::Max<float>(0, resultingAccel + GetGravityZ());
	float desiredTotalJetpackAccel = (GetGravityZ() * -1) + jetpackSurplusAccel;

	Velocity.Z += desiredTotalJetpackAccel * deltaTime;

	fJetpackResource = FMath::Clamp<float>(fJetpackResource - (deltaTime / MaxHoldJetpackTime), 0, fJetpackFullResource);
#pragma region print Velocity
	if (GetPawnOwner()->IsLocallyControlled())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString("Speed: ") + FString::SanitizeFloat(Velocity.Size()), true, false, FLinearColor::Red, 0.0);
		UKismetSystemLibrary::PrintString(GetWorld(), FString("Resource: ") + FString::SanitizeFloat(fJetpackResource), true, false, FLinearColor::Red, 0.0);
	}
#pragma endregion

	PhysFalling(deltaTime, Iterations);
}


#pragma region overriden methods
// Constructor
UMy_ShooterCharacterMovement::FNetworkPredictionData_Client_My::FNetworkPredictionData_Client_My(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

// This function allocates our Move.
FSavedMovePtr UMy_ShooterCharacterMovement::FNetworkPredictionData_Client_My::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_My());
}

// Get prediction data for a client game. Allocates the data on demand and can be overridden to allocate a custom override if desired.
FNetworkPredictionData_Client* UMy_ShooterCharacterMovement::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UMy_ShooterCharacterMovement* MutableThis = const_cast<UMy_ShooterCharacterMovement*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_My(*this);
	}

	return ClientPredictionData;
}

// This function saves the data from the Movement Component to the Move.
void UMy_ShooterCharacterMovement::FSavedMove_My::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UMy_ShooterCharacterMovement* CharacterMovement = Cast<UMy_ShooterCharacterMovement>(Character->GetCharacterMovement());

	if (CharacterMovement)
	{
		bSavedWantsToTeleport = CharacterMovement->bWantsToTeleport;
		SavedTeleportLocation = CharacterMovement->TeleportLocation;
		bSavedWantsToUseJetpack = CharacterMovement->bWantsToUseJetpack;
		SavedJetpackVelocity = CharacterMovement->Velocity.Z;
	}
}

// This function gets called when a move need to be replayed and puts the saved data from the Move to our Movement Component.
void UMy_ShooterCharacterMovement::FSavedMove_My::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UMy_ShooterCharacterMovement* CharacterMovement = Cast<UMy_ShooterCharacterMovement>(Character->GetCharacterMovement());

	if (CharacterMovement)
	{
		CharacterMovement->TeleportLocation = SavedTeleportLocation;
		CharacterMovement->Velocity.Z = SavedJetpackVelocity;
	}
}

// Function to decompress flags from a saved Move and to trigger the new ability to the server side
void UMy_ShooterCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	// Take the data from our custom flag and save it to our variable.
	bWantsToTeleport = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	bWantsToUseJetpack = (Flags & FSavedMove_Character::FLAG_Custom_2) != 0;
}

void UMy_ShooterCharacterMovement::FSavedMove_My::Clear()
{
	Super::Clear();

	// Reset the Move
	bSavedWantsToTeleport = false;

	bSavedWantsToUseJetpack = false;
}

// This function returns a byte containing our flags.
uint8 UMy_ShooterCharacterMovement::FSavedMove_My::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToTeleport)
	{
		Result |= FLAG_Custom_1;
	}

	if (bSavedWantsToUseJetpack)
	{
		Result |= FLAG_Custom_2;
	}

	return Result;
}

bool UMy_ShooterCharacterMovement::FSavedMove_My::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	// We tell to not send our moves combined with other ones
	if (bSavedWantsToTeleport != ((FSavedMove_My*)&NewMove)->bSavedWantsToTeleport)
	{
		return false;
	}

	if (bSavedWantsToUseJetpack != ((FSavedMove_My*)&NewMove)->bSavedWantsToUseJetpack)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

bool UMy_ShooterCharacterMovement::IsCustomMovementMode(uint8 customMove) const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == customMove;
}

void UMy_ShooterCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsCustomMovementMode(ECustomMovementMode::CMOVE_JETPACKING))
	{
		fJetpackResource = FMath::Clamp<float>(fJetpackResource + (DeltaTime / JetpackFullRechargeSeconds), 0, 1);
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString("Jetpack Resource: ") + FString::SanitizeFloat(fJetpackResource), true, false, FLinearColor::Red, 0.0);
	}


}

void UMy_ShooterCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	// Physic simulation shouldn't run on simulated proxy so we check and return it 
	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		return;
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Phys Custom");

	switch (CustomMovementMode)
	{
		case ECustomMovementMode::CMOVE_JETPACKING:
		{
			PhysJetpack(deltaTime, Iterations);
		}
	}


	Super::PhysCustom(deltaTime, Iterations);
}

// Function native of the standard Movement Component, this function is triggered at the end of a movement update.
void UMy_ShooterCharacterMovement::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (!CharacterOwner)
		return;

	// If the player has pressed the teleport key
	if (bWantsToTeleport)
	{
		bWantsToTeleport = false;

		// Sweep so we avoid collisions
		CharacterOwner->SetActorLocation(TeleportLocation, true);
	}

	if (bWantsToUseJetpack)
	{

		if (CanUseJetpack() == true)
		{
			SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_JETPACKING);
		}
		
	}

	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);
}

#pragma endregion