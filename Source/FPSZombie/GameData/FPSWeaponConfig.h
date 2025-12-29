// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSWeaponConfig.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UFPSWeaponConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UFPSWeaponConfig();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	TObjectPtr<class USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimSequence> FireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimSequence> ReloadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> CharacterFireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> CharacterADSFireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> CharacterReloadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> WeaponEquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float FireAnimMontagePlayRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float ReloadAnimMontagePlayRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponConfig)
	int32 MaxLoadedAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponConfig)
	uint8 bIsAutomatic : 1 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponConfig)
	float FireRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float DefaultFireSpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float MinRecoilPitch = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float MaxRecoilPitch = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float MinRecoilYaw = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float MaxRecoilYaw = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float GunshotDamage = 0.0f;
};
