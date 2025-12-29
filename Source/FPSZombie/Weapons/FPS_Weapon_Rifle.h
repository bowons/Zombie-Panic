// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FPSWeaponBase.h"
#include "FPS_Weapon_Rifle.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API AFPS_Weapon_Rifle : public AFPSWeaponBase
{
	GENERATED_BODY()

public:
	AFPS_Weapon_Rifle();

protected:
	virtual void AfterReloadAction() override;

};
