// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPS_WeaponPistol.h"

AFPS_WeaponPistol::AFPS_WeaponPistol()
{
	static ConstructorHelpers::FObjectFinder<UFPSWeaponConfig> WeaponDataRef(TEXT("/Script/FPSZombie.FPSWeaponConfig'/Game/FPSZombie/GameData/WeaponData/PistolData.PistolData'"));
	if (WeaponDataRef.Succeeded()) {
		WeaponData = WeaponDataRef.Object;

		WeaponMesh->SetSkeletalMesh(WeaponData->WeaponMesh);
	}

	WeaponMesh->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));

	//LoadedAmmo = WeaponData->MaxLoadedAmmo;
	//RemainingAmmo = 30;
	WeaponSlot = EWeaponSlot::Secondary;
	CurrentFireSpread = WeaponData->DefaultFireSpread;
}
