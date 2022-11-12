// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Weapons/MyShooterProjectile_Shrink.h"
#include "Weapons/MyShooterWeapon_Shrink.h"

AMyShooterWeapon_Shrink::AMyShooterWeapon_Shrink(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void AMyShooterWeapon_Shrink::ApplyWeaponConfig(FShrinkWeaponData& Data)
{
	Data = ShrinkProjectileConfig;

	
	if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Damage is " + (Data.DamageType->GetName()));
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Shrink time is " + (Data.ShrinkTime));

		}
}

void AMyShooterWeapon_Shrink::FireWeapon()
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetMuzzleLocation();

	// trace from camera to check what's under crosshair
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit

			FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			// spawn at crosshair position
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	ServerFireShrinkProjectile(Origin, ShootDir);
}

/** spawn projectile on server */
void AMyShooterWeapon_Shrink::ServerFireShrinkProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	FTransform SpawnTM(ShootDir.Rotation(), Origin);
	AMyShooterProjectile_Shrink* Projectile = Cast<AMyShooterProjectile_Shrink>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ShrinkProjectileConfig.ProjectileClass, SpawnTM));
	if (Projectile)
	{
		Projectile->SetInstigator(GetInstigator());
		Projectile->SetOwner(this);
		Projectile->InitVelocity(ShootDir);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}

bool AMyShooterWeapon_Shrink::ServerFireShrinkProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}