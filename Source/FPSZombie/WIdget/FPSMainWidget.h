// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UFPSMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LoadedAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* RemainingAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* StaminarBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* PrimaryWeaponImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* SecondaryWeaponImage;

	// Stat 수치 업데이트
	void UpdateAmmoHUD(const int32 UpdatedLoadedAmmo, const int32 UpdatedRemainingAmmo);
	void UpdateHealthHUD(const int32 UpdatedHP);
	void UpdateStaminarHUD(const float UpdatedStaminarPoint);
	void ChangeGunHUD(int32 WeaponSlot);

	// 무기 선택 이미지 크기 조정

protected:
	virtual void NativeConstruct() override;
};
