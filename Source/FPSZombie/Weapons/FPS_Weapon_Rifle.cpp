// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/FPS_Weapon_Rifle.h"
#include "Interfaces/CharacterAttachmentInterface.h"

AFPS_Weapon_Rifle::AFPS_Weapon_Rifle()
{
	static ConstructorHelpers::FObjectFinder<UFPSWeaponConfig> WeaponDataRef(TEXT("/Script/FPSZombie.FPSWeaponConfig'/Game/FPSZombie/GameData/WeaponData/RifleData.RifleData'"));
	if (WeaponDataRef.Succeeded()) {
		WeaponData = WeaponDataRef.Object;

		WeaponMesh->SetSkeletalMesh(WeaponData->WeaponMesh);
	}

	//LoadedAmmo = WeaponData->MaxLoadedAmmo;
	//RemainingAmmo = 60;
	WeaponSlot = EWeaponSlot::Primary;
	CurrentFireSpread = WeaponData->DefaultFireSpread;
}

void AFPS_Weapon_Rifle::AfterReloadAction()
{
	Super::AfterReloadAction();

	ICharacterAttachmentInterface* AttachTargetCharacter = Cast<ICharacterAttachmentInterface>(OwnerPlayer);
	if (AttachTargetCharacter) {
		AttachTargetCharacter->AttachWeapon("RifleSocket");
	}
}
