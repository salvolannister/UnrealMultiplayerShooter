// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Net/UnrealNetwork.h>

#include "Player/ShooterCharacter.h"

#include "My_ShooterCharacter.generated.h"

class UMy_ShooterCharacterMovement;
class UMyActorShrinkComponent;

UCLASS()
class SHOOTERGAME_API AMy_ShooterCharacter : public AShooterCharacter
{
	GENERATED_BODY()
	
public:
	// With this constructor Unreal will automatically initialize this Character with my new Movement Component
	AMy_ShooterCharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Shrink")
	UMyActorShrinkComponent* ShrinkComponent;

	// Function for get our Movement Component without casting
	UFUNCTION(BlueprintCallable, Category = "Movement")
    UMy_ShooterCharacterMovement* GetMyMovementComponent();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerTakeWeapon(AMyShooterPickup_Gun *weapon);

	UFUNCTION(NetMulticast, reliable)
	void MulticastRPCDestroyWeapon(AMyShooterPickup_Gun* weapon);

	// Called every frame
	UPROPERTY(ReplicatedUsing = OnRep_IsShrinked)
	bool bIsShrinked;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsShrinked)
	bool bSkipShrinkInterpolation = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsShrinked)
	float fShrinkTime;
	/* Gets if the player is shrinked*/
	UFUNCTION()
	bool IsShrinked();

	//Replicates mesh changes on clients
	UFUNCTION()
	void OnRep_IsShrinked();

	UFUNCTION(unreliable, server, WithValidation)
	void ServerShrink(bool shrink,float shrinkTime = 0, bool skipInterpolation = false);

	/** [server RPC] Drops the currently equipped gun as pickup */
	UFUNCTION(reliable, server, WithValidation)
	void ServerDropWeapon();

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

private:
	// Input binding is set in the character in the original project, overriding this method let us expand those bindings
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Function that will call the Teleport implementation in the Movement Component
	void Teleport();
	// Function that will call teh Jetpack implementation in the Movement Component
	void UseJetpack();
	// Function called when the jetpacking button is realesed
	void StopJetpacking();
	// Function called when the player dies and has a weapon with ammo greater than zero
	void DropWeapon();
	// Function called when the player picks up a weapon
	void PickWeapon();

	//Function that updates the variables related to the shrink effect on the character
	void Shrink(bool shrink,float fShrinkTime, bool skipInterpolation = false);
	
	// Keeps a copy of the character movement, uproperty ensure the data is handled by Unreal GC
	UPROPERTY()
	UMy_ShooterCharacterMovement* ShooterCharacterMovement;

	TSubclassOf<class AShooterPickup_Ammo> ShooterPickupDroppedGunClass;

#pragma region weapon
	/** Weapon type: rifle */
	TSubclassOf<class AShooterWeapon> WeaponTypeRifle;

	/** Weapon type: rocket launcher */
	
	TSubclassOf<class AShooterWeapon> WeaponTypeRocketLauncher;
	/** Weapon type is a shrink rocket launcher*/
	
	TSubclassOf<class AShooterWeapon> WeaponTypeShrinkLauncher;
	// Add new weapon here
#pragma endregion

protected:

	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
};


