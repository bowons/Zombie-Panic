// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/FPSImpactInfoData.h"
#include "GameData/FPSWeaponConfig.h"
#include "WIdget/FPSMainWidget.h"
#include "InputActionValue.h"
#include "FPSWeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8 {
	Primary = 0 UMETA(DisplayName = "Primary Weapon"),
	Secondary = 1 UMETA(DisplayName = "Secondary Weapon")
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedDelegate, const int32, const int32);
DECLARE_DELEGATE_OneParam(FReloadingActionDelegate, const FInputActionValue&);

UCLASS()
class FPSZOMBIE_API AFPSWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFPSWeaponBase();

protected:
	void PostInitializeComponents() override;

public:
	void Fire();
	void Reload();
	void GetAmmo(uint8 NewAmmo);
	void ValidateHUD();

	FORCEINLINE void StartFiring() { bIsFiring = true; };
	FORCEINLINE void StopFiring() { bIsFiring = false; };
	FORCEINLINE void SetAimed() { CurrentFireSpread = 0.0f; }
	FORCEINLINE void UnAim() { CurrentFireSpread = WeaponData->DefaultFireSpread * 0.05f; }
	
	FORCEINLINE uint8 isReloading() { return bIsReloading; };
	FORCEINLINE uint8 isFiring() { return bIsFiring; };
	FORCEINLINE uint8 isAutomatic() { return bIsAutomatic; };
	FORCEINLINE uint8 isNoAmmo() { return RemainingAmmo <= 0; };
	FORCEINLINE uint8 isFullAmmo() { return LoadedAmmo == WeaponData->MaxLoadedAmmo; };
	FORCEINLINE uint8 isNotCoolDown() { return (GetWorld()->GetTimeSeconds() - TimeSinceLastShot) >= WeaponData->FireRate; };
	
	FORCEINLINE TObjectPtr<class UAnimMontage> GetEquipAnim() { return WeaponData->WeaponEquipAnim; }
	FORCEINLINE TObjectPtr<class UAnimMontage> GetADSFireAnim() { return WeaponData->CharacterADSFireAnim; }
	FORCEINLINE TObjectPtr<class UAnimMontage> GetFireAnim() { return WeaponData->CharacterFireAnim; }
	
	void SetupAmmoHUD(UFPSMainWidget* AmmoWidget);

public:
	FORCEINLINE TObjectPtr<class USkeletalMeshComponent> GetWeaponMesh() { return WeaponMesh; }
	FORCEINLINE EWeaponSlot GetWeaponType() { return WeaponSlot; }
	FOnAmmoChangedDelegate OnAmmoChanged;
	FReloadingActionDelegate OwnerReloadingAction;

protected:
	void WeaponFireLineTrace();
	void DrawBulletImpact(TWeakObjectPtr<class UPhysicalMaterial> SurfaceMaterial, FVector HitNormal, FVector HitLocation);
	virtual void AfterReloadAction();
	void AddRecoil();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	TObjectPtr<class AFPSCharacterPlayer> OwnerPlayer;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponData)
	TObjectPtr<class UFPSWeaponConfig> WeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UFPSPlayerAniminstance> PlayerArmAnimInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UNiagaraSystem> FireTrailSystem;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int32 LoadedAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
	int32 RemainingAmmo = 0;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	EWeaponSlot WeaponSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	float CurrentFireSpread = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	uint8 bIsFiring : 1 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	uint8 bIsReloading : 1 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	uint8 bIsAutomatic : 1 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponConfig)
	double TimeSinceLastShot = 0.0f;
	
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactInfo> ImpactInfoMapBySurface;
};