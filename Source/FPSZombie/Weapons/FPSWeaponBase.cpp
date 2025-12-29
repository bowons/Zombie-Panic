// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FPSWeaponBase.h"
#include "Character/FPSCharacterPlayer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Animations/FPSPlayerAniminstance.h"
#include "Engine/DamageEvents.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSWeaponBase::AFPSWeaponBase()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));
	WeaponMesh->SetCastShadow(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireTrailSystemRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/LyraAssets/Effects/Particles/Weapons/NS_WeaponFire_Tracer.NS_WeaponFire_Tracer'"));
	if (FireTrailSystemRef.Succeeded())
		FireTrailSystem = FireTrailSystemRef.Object;
	
	static ConstructorHelpers::FObjectFinder<UFPSImpactInfoData> ImpactInfoDataRef(TEXT("/Script/FPSZombie.FPSImpactInfoData'/Game/FPSZombie/GameData/FPSImpactInfoDataAsset.FPSImpactInfoDataAsset'"));
	if (ImpactInfoDataRef.Succeeded()) {
		ImpactInfoMapBySurface = ImpactInfoDataRef.Object->ImpactInfoMap;
	}
}

void AFPSWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OwnerPlayer = Cast<AFPSCharacterPlayer>(GetOwner());
	if (OwnerPlayer) {
		PlayerArmAnimInstance = Cast<UFPSPlayerAniminstance>(OwnerPlayer->GetCharacterArmInstance());
	}

	bIsAutomatic = WeaponData->bIsAutomatic;
	CurrentFireSpread *= 0.05f;
}

void AFPSWeaponBase::Fire()
{
	UAnimMontage* CharacterFireAnim = OwnerPlayer->IsAiming() ? WeaponData->CharacterADSFireAnim : WeaponData->CharacterFireAnim;

	if (LoadedAmmo > 0) {
		PlayerArmAnimInstance->Montage_Play(CharacterFireAnim, WeaponData->FireAnimMontagePlayRate);
		WeaponMesh->PlayAnimation(WeaponData->FireAnim, false);
		WeaponFireLineTrace();
		TimeSinceLastShot = GetWorld()->GetTimeSeconds();
		AddRecoil();
		OwnerPlayer->FiringCamShake();
		OnAmmoChanged.Broadcast(--LoadedAmmo, RemainingAmmo);
	}
	else {
		OwnerReloadingAction.Execute(FInputActionValue(true));
	}
}

void AFPSWeaponBase::Reload()
{
	if (bIsReloading)
		return;

	if (isNoAmmo() || isFullAmmo())
		return;

	StopFiring();
	bIsReloading = true;

	PlayerArmAnimInstance->Montage_Play(WeaponData->CharacterReloadAnim, WeaponData->ReloadAnimMontagePlayRate);
	// 애니메이션이 끝나면 실행될 임시 함수를 바인딩함
	FOnMontageEnded ReloadEndDelegate;
	ReloadEndDelegate.BindLambda(
		[this](UAnimMontage* Montage, bool bInterrupted) 
		{
			bIsReloading = false;
			AfterReloadAction();
		}
	);
		
	PlayerArmAnimInstance->Montage_SetEndDelegate(ReloadEndDelegate, WeaponData->CharacterReloadAnim);

	WeaponMesh->PlayAnimation(WeaponData->ReloadAnim, false);
}

void AFPSWeaponBase::GetAmmo(uint8 NewAmmo)
{
	RemainingAmmo += NewAmmo;
	OnAmmoChanged.Broadcast(LoadedAmmo, RemainingAmmo);
}

void AFPSWeaponBase::ValidateHUD()
{
	OnAmmoChanged.Broadcast(LoadedAmmo, RemainingAmmo);
}

void AFPSWeaponBase::WeaponFireLineTrace()
{
	FVector CamForwardVector = OwnerPlayer->GetCameraForwardVector();
	float FireConeSpreadAngleRadians = FMath::DegreesToRadians(CurrentFireSpread);

	FVector FireDirection = FMath::VRandCone(CamForwardVector, FireConeSpreadAngleRadians);
	FVector TraceTargetLocation = OwnerPlayer->GetCameraWorldLocation() + (FireDirection * 10000.0f);

	UWorld* World = GetWorld();
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerPlayer);

	bool Result = World->LineTraceSingleByChannel(
		HitResult,
		OwnerPlayer->GetCameraWorldLocation(),
		TraceTargetLocation,
		ECollisionChannel::ECC_Visibility,
		Params
	);

	UAISense_Hearing::ReportNoiseEvent(
		World,
		OwnerPlayer->GetCameraWorldLocation(),
		1.0f,
		OwnerPlayer,
		0.0f,
		FName("GunShot")
	);

	FVector HitLocation;
	FVector HitNormal;
	TWeakObjectPtr<UPhysicalMaterial> HitPhysMat;
	if (Result) {
		HitLocation = HitResult.Location;
		HitNormal = HitResult.Normal;
		HitPhysMat = HitResult.PhysMaterial;

		AFPSCharacterBase* HitCharacter = Cast<AFPSCharacterBase>(HitResult.GetActor());
		
		if (HitCharacter) {
			//FDamageEvent DamageEvent;
			//HitCharacter->TakeDamage(WeaponData->GunshotDamage, DamageEvent, OwnerPlayer->GetController(), OwnerPlayer);
			UGameplayStatics::ApplyPointDamage(
				HitCharacter,
				WeaponData->GunshotDamage,
				HitLocation,
				HitResult,
				OwnerPlayer->GetController(),
				OwnerPlayer,
				UDamageType::StaticClass()
			);
		}
	}
	else {
		HitLocation = TraceTargetLocation;
		HitNormal = FVector::ZeroVector;
		HitPhysMat = nullptr;
	}
	
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(TEXT("MuzzleSocket"));
	
	UNiagaraComponent* FireTrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		FireTrailSystem,
		WeaponMesh,
		TEXT("MuzzleSocket"),
		MuzzleLocation,
		MuzzleRotation,
		EAttachLocation::KeepWorldPosition,
		true,
		false,
		ENCPoolMethod::None,
		true
	);
	
	TArray<FVector> ImpactPositionVector;
	ImpactPositionVector.Add(HitLocation);

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(FireTrailComponent, TEXT("User.ImpactPositions"), ImpactPositionVector);
	FireTrailComponent->SetVectorParameter(TEXT("User.MuzzlePosition"), MuzzleLocation);
	FireTrailComponent->SetBoolParameter(TEXT("User.Trigger"), true);
	
	FireTrailComponent->Activate();

	DrawBulletImpact(HitPhysMat, HitNormal, HitLocation);
}

void AFPSWeaponBase::DrawBulletImpact(TWeakObjectPtr<UPhysicalMaterial> SurfaceMaterial, FVector HitNormal, FVector HitLocation)
{

	if (SurfaceMaterial == nullptr)
		return;

	FImpactInfo* ImpactInfoBySurface =  ImpactInfoMapBySurface.Find(SurfaceMaterial->SurfaceType);
	TObjectPtr<UNiagaraSystem> ImpactDecal;
	TObjectPtr<USoundBase> ImpactSound;

	if (!ImpactInfoBySurface) {
		ImpactInfoBySurface = &ImpactInfoMapBySurface[EPhysicalSurface::SurfaceType1];
	}

	ImpactDecal = ImpactInfoBySurface->ImpactDecal;
	ImpactSound = ImpactInfoBySurface->ImpactSound;

	FVector OffsetLocation = HitLocation + HitNormal * 1.25f;
	UNiagaraComponent* BulletImpactComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), ImpactDecal, OffsetLocation, FRotator::ZeroRotator, ((FVector)(1.0F)), true, false);

	BulletImpactComponent->SetVectorParameter(TEXT("User.ImpactNormal"), HitNormal);
	BulletImpactComponent->SetVectorParameter(TEXT("User.ImpactPosition"), OffsetLocation);

	BulletImpactComponent->Activate();


	FTimerHandle TimerHandle;
	// 0.1초 후에 사운드를 재생하도록 타이머를 설정
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, ImpactSound, OffsetLocation]()
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, OffsetLocation, 1.0f, 1.0f);
		}, 0.1f, false);
	
}

void AFPSWeaponBase::AfterReloadAction()
{
	// 이 함수는 상속되는 용도로 선언되었음, Reloading 애니메이션이 완료된 후 해당 작업이 실행됨, 탄창 재장전과 Ammo 업데이트 또한 수행함
	int32 BringAmmo = WeaponData->MaxLoadedAmmo - LoadedAmmo;
	BringAmmo = BringAmmo > RemainingAmmo ? RemainingAmmo : BringAmmo;
	
	RemainingAmmo -= BringAmmo;
	LoadedAmmo += BringAmmo;
	
	OnAmmoChanged.Broadcast(LoadedAmmo, RemainingAmmo);
}

void AFPSWeaponBase::AddRecoil()
{
	float Multiplyer = OwnerPlayer->IsAiming() ? 0.25f : 0.5;

	float VerticalRecoil = FMath::FRandRange(WeaponData->MinRecoilPitch, WeaponData->MaxRecoilPitch) * Multiplyer;
	float HorizontalRecoil = FMath::FRandRange(WeaponData->MinRecoilYaw, WeaponData->MaxRecoilYaw) * Multiplyer;
	OwnerPlayer->ReceiveGunRecoil(VerticalRecoil, HorizontalRecoil);
}

void AFPSWeaponBase::SetupAmmoHUD(UFPSMainWidget* AmmoWidget)
{
	UFPSMainWidget* AmmoHUD = Cast<UFPSMainWidget>(AmmoWidget);
	if (AmmoHUD) {
		AmmoHUD->UpdateAmmoHUD(LoadedAmmo, RemainingAmmo);
		OnAmmoChanged.RemoveAll(AmmoHUD);
		OnAmmoChanged.AddUObject(AmmoHUD, &UFPSMainWidget::UpdateAmmoHUD);
	}
}

