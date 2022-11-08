// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Pickups/MyShooterPickup_Gun.h"
#include "OnlineSubsystemUtils.h"
#include <My_ShooterCharacter.h>

/* The MaskMesh object is replicated setting to "Replicate" in the property, using the method "GetLifetimeReplicatedProps" and ensuring that the
constructor sets bReplicates to true*/

AMyShooterPickup_Gun::AMyShooterPickup_Gun(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AmmoClips = 0;
	AmmoLoadedClip = 0;

	PickupMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	bReplicates = true;
	RespawnTime = 0;
}


void AMyShooterPickup_Gun::OnRep_MaskMesh()
{
	SetWeaponPickupMesh(MaskMesh);
}

void AMyShooterPickup_Gun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tells the network when to replicate the variables
	DOREPLIFETIME(AMyShooterPickup_Gun, MaskMesh);
}

void AMyShooterPickup_Gun::SetAmmoClips(int32 Clips)
{
	AmmoClips = Clips;
}

void AMyShooterPickup_Gun::SetAmmoLoadedClip(int32 LoadedClips)
{
	AmmoLoadedClip = LoadedClips;
}
void AMyShooterPickup_Gun::SetWeaponType(TSubclassOf<AShooterWeapon> Type)
{
	WeaponType = Type;
}
void AMyShooterPickup_Gun::SetWeaponPickupMesh(USkeletalMesh* WeaponMesh)
{
	PickupMesh->SetSkeletalMesh(WeaponMesh, false);
}


void AMyShooterPickup_Gun::GivePickupTo(class AShooterCharacter* Pawn)
{
	AShooterWeapon* Weapon = (Pawn ? Pawn->FindWeapon(WeaponType) : NULL);
	if (Weapon)
	{
		int32 Qty = AmmoClips * Weapon->GetAmmoPerClip() + AmmoLoadedClip;
		Weapon->GiveAmmo(Qty);

		// Fire event for collected ammo
		if (Pawn)
		{
			const UWorld* World = GetWorld();
			const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
			const IOnlineIdentityPtr Identity = Online::GetIdentityInterface(World);

			if (Events.IsValid() && Identity.IsValid())
			{
				AShooterPlayerController* PC = Cast<AShooterPlayerController>(Pawn->Controller);
				if (PC)
				{
					ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);

					if (LocalPlayer)
					{
						const int32 UserIndex = LocalPlayer->GetControllerId();
						TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
						if (UniqueID.IsValid())
						{
							FVector Location = Pawn->GetActorLocation();

							FOnlineEventParms Params;

							Params.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
							Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
							Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

							Params.Add(TEXT("ItemId"), FVariantData((int32)Weapon->GetAmmoType() + 1)); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
							Params.Add(TEXT("AcquisitionMethodId"), FVariantData((int32)0)); // unused
							Params.Add(TEXT("LocationX"), FVariantData(Location.X));
							Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
							Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));
							Params.Add(TEXT("ItemQty"), FVariantData((int32)Qty));

							Events->TriggerEvent(*UniqueID, TEXT("CollectPowerup"), Params);
						}
					}
				}
			}
		}
	}
}


void AMyShooterPickup_Gun::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeTime);
}

void AMyShooterPickup_Gun::OnPickedUp()
{
	Super::OnPickedUp();

	// Since the base class Actor it's already set to be replicated and U4 has no poblem in
	// replicating a destroy we could just call Destroy. 
	// This was my effort to avoid seeing the gun going away after few seconds because there is no client prediction implemented
	AMy_ShooterCharacter* MSC = Cast<AMy_ShooterCharacter>(PickedUpBy);
	if (MSC != NULL)
	{
		if (MSC->GetLocalRole() == ROLE_Authority)
		{
			Destroy();
		}
		else
		{
			MSC->ServerTakeWeapon(this);
		}
	}
}




