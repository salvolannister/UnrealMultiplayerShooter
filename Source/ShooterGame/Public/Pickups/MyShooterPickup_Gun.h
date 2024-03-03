// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "Weapons/ShooterWeapon.h"
#include "Pickups/ShooterPickup_Ammo.h"

#include "MyShooterPickup_Gun.generated.h"

/**
 *
 */
class UMaterialInterface;
 //Class that stores the data of the pickup weapon 
UCLASS(Abstract, Blueprintable)
class SHOOTERGAME_API AMyShooterPickup_Gun : public AShooterPickup_Ammo
{
	GENERATED_UCLASS_BODY()


public:

	//Enable replication of mesh change also 
	UPROPERTY(ReplicatedUsing = OnRep_MaskMesh)
	USkeletalMesh* MaskMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Materials)
	TArray<UMaterialInterface*> GunMaterials;

	//Replicates mesh changes on clients
	UFUNCTION()
	void OnRep_MaskMesh();

	//Replicates material changes on clients
	UFUNCTION()
	void OnRep_Materials();

	/** Set how many clips this pickup holds */
	void SetAmmoClips(int32 Clips);

	/** Set how many loaded clips this pickup holds */
	void SetAmmoLoadedClip(int32 LoadedClips);

	/** Set the weapon type */
	void SetWeaponType(TSubclassOf<AShooterWeapon> Type);

	/** Set the weapon mesh */
	void SetWeaponPickupMesh(USkeletalMesh* WeaponMesh);

	/** Set Materials of the weapon to spawn */
	void SetWeaponMaterials(TArray<UMaterialInterface*> GunMaterials);

private:

	/** Mesh component */
	//TODO: what about using replicate?
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* PickupMesh;

protected:

	virtual void BeginPlay() override;

	virtual void OnPickedUp() override;

	virtual void GivePickupTo(AShooterCharacter* Pawn) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/** How much ammo there are in the currently loaded clip? */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	int32 AmmoLoadedClip;

	/** how much this pickup lives? */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	float LifeTime = 100.0f;

	/** Returns Pickup Mesh Component subobject **/
	FORCEINLINE USkeletalMeshComponent* GetPickupMesh() const { return PickupMesh; }


};
