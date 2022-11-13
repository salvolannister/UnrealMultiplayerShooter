// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "My_ShooterCharacter.h"
#include "My_ShooterCharacterMovement.h"
#include "MyShooterPickup_Gun.h"

// With this constructor Unreal will automatically initialize this Character with my new Movement Component
AMy_ShooterCharacter::AMy_ShooterCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UMy_ShooterCharacterMovement>(ACharacter::CharacterMovementComponentName))
{

	// Finds the class for the dropped weapon pickup on player death
	static ConstructorHelpers::FClassFinder<AShooterPickup_Ammo> BPClassFinder(TEXT("/Game/Blueprints/Pickups/Pickup_DroppedWeapon"));
	ShooterPickupDroppedGunClass = BPClassFinder.Class;
	static ConstructorHelpers::FClassFinder<AShooterWeapon> BPClassFinder2(TEXT("/Game/Blueprints/Weapons/WeapGun"));
	WeaponTypeRifle = BPClassFinder2.Class;
	static ConstructorHelpers::FClassFinder<AShooterWeapon> BPClassFinder3(TEXT("/Game/Blueprints/Weapons/WeapLauncher"));
	WeaponTypeRocketLauncher = BPClassFinder3.Class;
	static ConstructorHelpers::FClassFinder<AShooterPickup_Ammo> BPClassFinder4(TEXT("/Game/Blueprints/Weapons/WeapShrinker"));
	WeaponTypeShrinkLauncher = BPClassFinder4.Class;

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

#pragma region weapon

void AMy_ShooterCharacter::DropWeapon()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform SpawnTS(GetActorRotation(), GetActorLocation());
	//Instantiates the pickup ammo
	AMyShooterPickup_Gun* DroppedGun = Cast<AMyShooterPickup_Gun>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ShooterPickupDroppedGunClass, SpawnTS, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));


	if (DroppedGun)
	{
		//Sets the mesh and clips
		AShooterWeapon* EquippedWeapon = GetWeapon();
		const int32 CurrentAmmo = EquippedWeapon->GetCurrentAmmo();
		DroppedGun->SetAmmoClips(CurrentAmmo / EquippedWeapon->GetAmmoPerClip());
		DroppedGun->SetAmmoLoadedClip(CurrentAmmo % EquippedWeapon->GetAmmoPerClip());
		DroppedGun->MaskMesh = EquippedWeapon->GetWeaponMesh()->SkeletalMesh; // mask variable to trigger OnRep call
		DroppedGun->SetWeaponPickupMesh(DroppedGun->MaskMesh); // update mesh when called by the server
		TSubclassOf<class AShooterWeapon> WeapType;
		switch (EquippedWeapon->GetAmmoType())
		{
		case AShooterWeapon::EAmmoType::EBullet:
			WeapType = WeaponTypeRifle;
			break;
		case AShooterWeapon::EAmmoType::ERocket:
			WeapType = WeaponTypeRocketLauncher;
			break;
		}
		DroppedGun->SetWeaponType(WeapType);

		UGameplayStatics::FinishSpawningActor(DroppedGun, SpawnTS);
	}

}

void AMy_ShooterCharacter::ServerTakeWeapon_Implementation(AMyShooterPickup_Gun* gun)
{
	MulticastRPCDestroyWeapon(gun);

}

void AMy_ShooterCharacter::MulticastRPCDestroyWeapon_Implementation(AMyShooterPickup_Gun* gun)
{

	if (IsValid(gun))
	{
		GetWorld()->DestroyActor(gun);
	}


}

bool AMy_ShooterCharacter::ServerTakeWeapon_Validate(AMyShooterPickup_Gun* gun)
{
	return true;
}

bool AMy_ShooterCharacter::ServerDropWeapon_Validate()
{
	return true;
}

void AMy_ShooterCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}

#pragma endregion 


void AMy_ShooterCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority)
		DropWeapon();
	else
		ServerDropWeapon();

	Super::OnDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);
}

float AMy_ShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) 
{
	bool bIsShrinkDamage = (DamageEvent.DamageTypeClass->GetName()).Equals("DmgType_Shrink_C");
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Inside take damage ");

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Damage type "+ DamageEvent.DamageTypeClass->GetName());
	


	if (bIsShrinkDamage )
	{
		Damage = 0;
		UMy_ShooterCharacterMovement* SCM = GetMyMovementComponent();
		
		if (SCM->IsShrinked() == false) 
		{
			// Adjust camera position
		    // check shrink state
			float fShrinkTime = Damage;
			SCM->SetShrinkedState(true, fShrinkTime);
				// set shrink state
		   
		   
		}
			
		
		
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Actor scale is " + GetActorScale().ToString());
		
	}
	
	//Call it here so if PC is in God Mode he/she returns
	float const ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	return ActualDamage;
	
}

// Function for get our Movement Component 
UMy_ShooterCharacterMovement* AMy_ShooterCharacter::GetMyMovementComponent() const
{
	return Cast<UMy_ShooterCharacterMovement>(GetCharacterMovement());
}

