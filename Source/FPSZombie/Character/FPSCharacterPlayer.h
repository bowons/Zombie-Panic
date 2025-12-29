// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FPSCharacterBase.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameData/FPSCharacterInputData.h"
#include "Camera/CameraComponent.h"
#include "Weapons/FPSWeaponBase.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/CharacterAttachmentInterface.h"
#include "Interfaces/PlayerHUDInterface.h"
#include "FPSCharacterPlayer.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnStaminarChangedDelegate, const float);

UCLASS()
class FPSZOMBIE_API AFPSCharacterPlayer : public AFPSCharacterBase, public ICharacterAttachmentInterface, public IPlayerHUDInterface
{
	GENERATED_BODY()

public:
	AFPSCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void SetupMainHUD(UFPSMainWidget* MainHUD) override;
	void FiringCamShake();

	void PlaySound(USoundBase* TargetSound, float StartTime = 0.0f);
	virtual void AttachWeapon(FName SocketName) override;
	virtual void SwapWeapon(EWeaponSlot WeaponType) override;
	void ReceiveGunRecoil(float VerticalRecoil, float HorizontalRecoil);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetLevel(int32 Round) override;

protected:
	void CharacterLook(const FInputActionValue& Value);
	void CharacterMove(const FInputActionValue& Value);
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void StopFiring(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);
	void StartAiming(const FInputActionValue& Value);
	void StopAiming(const FInputActionValue& Value);
	void PressNum1(const FInputActionValue& Value);
	void PressNum2(const FInputActionValue& Value);

public:
	FORCEINLINE TObjectPtr<class UAnimInstance> GetCharacterArmInstance() { return PlayerArmSkeletal->GetAnimInstance(); }
	FORCEINLINE FVector GetCameraForwardVector() { return FPSCamera->GetForwardVector(); }
	FORCEINLINE FVector GetCameraWorldLocation() { return FPSCamera->GetComponentLocation(); }
	FORCEINLINE bool IsAiming() { return bIsAiming; }

private:
	void SetupMeshAndShadows();
	void SetupDefaultMovementAndInput();
	void SetupPlayerAudioCopmonents();

private:
	void HideHeadandArmMesh();
	void AttachWeapon();

	UFUNCTION()
	void HandleRoundStarted(int32 RoundIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UCameraComponent> FPSCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skeletal)
	TObjectPtr<class USkeletalMeshComponent> PlayerArmSkeletal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow)
	USkeletalMeshComponent* ShadowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow)
	USkeletalMeshComponent* WeaponShadowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	TObjectPtr<class AFPSWeaponBase> SelectedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	UFPSMainWidget* MainHUD;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InputConfig)
	TObjectPtr<class UFPSCharacterInputData> InputConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InputConfig)
	UCurveFloat* AimingCurve;

	FTimeline AimingTimeline;
	
	UFUNCTION()
	void AimUpdate(float Alpha);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	UFPSPlayerAniminstance* PlayerArmAnimInstance;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	TObjectPtr<class UAudioComponent> PlayerAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	TObjectPtr<class USoundAttenuation> AudioSoundAttenuationSetting;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	uint8 bIsSprinting : 1 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	uint8 bIsAiming : 1 =  0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	uint8 bIsSwaping : 1 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float StaminarPoint = 100.0f;

	uint8 bJogCoolDown : 1 = false;

	float MovementGravityScale = 2.0f;
	float MovementJumpZVelocity = 600.0f;
	float MovementAimingSpeed = 150.0f;
	float MovementWalkSpeed = 300.0f;
	float MovementJogSpeed = 600.0f;
	float MovementWalkSpeedCrouch = 150.0f;

	float CurrentVerticalRecoil = 0.0f;
	float CurrentHorizontalRecoil = 0.0f;

	float TargetVerticalRecoil = 0.0f;
	float TargetHorizontalRecoil = 0.0f;

	void ResetRecoil();
	void GetRebound(float DeltaTime);

	FOnStaminarChangedDelegate OnStaminarChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

protected:
	TMap<EWeaponSlot, TObjectPtr<class AFPSWeaponBase>> OwningWeapons;
};
