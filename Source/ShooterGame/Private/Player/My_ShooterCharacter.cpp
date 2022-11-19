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
	static ConstructorHelpers::FClassFinder<AShooterWeapon> BPClassFinder4(TEXT("/Game/Blueprints/Weapons/WeapShrinker"));
	WeaponTypeShrinkLauncher = BPClassFinder4.Class;

	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}


void AMy_ShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	ShrinkComponent = FindComponentByClass<UMyActorShrinkComponent>();
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		OnActorHit.AddDynamic(this, &AMy_ShooterCharacter::OnHit);
	}

}


void AMy_ShooterCharacter::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{

	if (ShrinkComponent && bIsShrinked && OtherActor->IsA(AMy_ShooterCharacter::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, " KILLED ");
		// (float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
		AMy_ShooterCharacter* OtherSC = Cast<AMy_ShooterCharacter>(OtherActor);
		if (OtherSC != NULL)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, " cast success ");
			AController* Killer = OtherSC->GetController();

			FDamageEvent* DamageEvent = new FDamageEvent();
			// It's already executed only on server thanks to CanDie() function
			Die(0, *DamageEvent, Killer, OtherActor);
		}

	}
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
		USkeletalMeshComponent* EquippedWeaponMeshComponent = EquippedWeapon->GetWeaponMesh();
		DroppedGun->MaskMesh = EquippedWeaponMeshComponent->SkeletalMesh; // mask variable to trigger OnRep call
		DroppedGun->Materials = EquippedWeaponMeshComponent->GetMaterials();
		DroppedGun->SetWeaponPickupMesh(DroppedGun->MaskMesh); // update mesh when called by the server
		DroppedGun->SetWeaponMaterials(DroppedGun->Materials);
		TSubclassOf<class AShooterWeapon> WeapType;
		switch (EquippedWeapon->GetAmmoType())
		{
			case AShooterWeapon::EAmmoType::EBullet:
				WeapType = WeaponTypeRifle;
				break;
			case AShooterWeapon::EAmmoType::ERocket:
				WeapType = WeaponTypeRocketLauncher;
				break;
			case AShooterWeapon::EAmmoType::EShrinkRocket:
				WeapType = WeaponTypeShrinkLauncher;
		}
		DroppedGun->SetWeaponType(WeapType);

		UGameplayStatics::FinishSpawningActor(DroppedGun, SpawnTS);
	}

}
void AMy_ShooterCharacter::OnRep_IsShrinked()
{
	if (ShrinkComponent != NULL)
	{
		ShrinkComponent->Shrink(bIsShrinked);
	}
}

void AMy_ShooterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tells the network when to replicate the variables
	DOREPLIFETIME(AMy_ShooterCharacter, bIsShrinked);
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
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Damage type " + DamageEvent.DamageTypeClass->GetName());



	if (bIsShrinkDamage)
	{



		UMy_ShooterCharacterMovement* SCM = GetMyMovementComponent();


		// Adjust camera position
		// check shrink state
		float fShrinkTime = Damage;

		ShrinkComponent->SetTime(fShrinkTime);
		if (GetOwner()->GetLocalRole() >= ENetRole::ROLE_Authority)
			//Setting the time won't change anything until the server will update the shrinked state
			// set shrink state
		{
			ServerShrink(true);
		}

		Damage = 0;
	}


	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Actor scale is " + GetActorScale().ToString());



	//Call it here so if PC is in God Mode he/she returns
	float const ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	return ActualDamage;

}

void AMy_ShooterCharacter::ServerShrink_Implementation(bool shrink, float time = 0)
{
	bIsShrinked = shrink;
	OnRep_IsShrinked();
}

bool AMy_ShooterCharacter::IsShrinked()
{
	return bIsShrinked;
}

bool AMy_ShooterCharacter::ServerShrink_Validate(bool shrink, float time = 0)
{
	return true;
}


// Function for get our Movement Component 
UMy_ShooterCharacterMovement* AMy_ShooterCharacter::GetMyMovementComponent() const
{
	return Cast<UMy_ShooterCharacterMovement>(GetCharacterMovement());
}

