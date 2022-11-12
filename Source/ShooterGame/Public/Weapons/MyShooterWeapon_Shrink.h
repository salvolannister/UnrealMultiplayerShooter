// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Weapons/ShooterWeapon.h"
#include "GameFramework/DamageType.h" // for UDamageType::StaticClass()
#include "MyShooterWeapon_Shrink.generated.h"

USTRUCT()
struct FShrinkWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** projectile class */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMyShooterProjectile_Shrink> ProjectileClass;

	/** life time */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ProjectileLife;

	/** Time the opponent willbe shrinked */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 ShrinkTime;

	/** radius of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float ExplosionRadius;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	/** defaults */
	FShrinkWeaponData()
	{
		ProjectileClass = NULL;
		ProjectileLife = 10.0f;
		ShrinkTime = 60; 
		ExplosionRadius = 300.0f;
		DamageType = NULL;
	}
};
/**
 * A weapon that fires a visible projectile that will shrink all the player in a certain radius for a certain amount of time.
 * If during this time another player stomps on him, then
he dies.
 */
UCLASS(Abstract)
class SHOOTERGAME_API AMyShooterWeapon_Shrink : public AShooterWeapon
{
	GENERATED_UCLASS_BODY()
	
	/** apply config on projectile */
	void ApplyWeaponConfig(FShrinkWeaponData& Data);

protected:

	virtual EAmmoType GetAmmoType() const override
	{
		return EAmmoType::EShrinkRocket;
	}

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FShrinkWeaponData ShrinkProjectileConfig;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;

	/** spawn projectile on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireShrinkProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);
};
