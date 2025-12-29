// Fill out your copyright notice in the Description page of Project Settings.


#include "WIdget/FPSMainWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Weapons/FPSWeaponBase.h"
#include "Interfaces/PlayerHUDInterface.h"

void UFPSMainWidget::UpdateAmmoHUD(const int32 UpdatedLoadedAmmo, const int32 UpdatedRemainingAmmo)
{
	if (LoadedAmmo) {
		LoadedAmmo->SetText(FText::FromString(FString::FromInt(UpdatedLoadedAmmo)));
	}

	if (RemainingAmmo) {
		RemainingAmmo->SetText(FText::FromString(FString::FromInt(UpdatedRemainingAmmo)));
	}
}

void UFPSMainWidget::UpdateHealthHUD(const int32 UpdatedHP)
{
	if (HPBar) {
		float Percent = UpdatedHP / 100.0f;
		HPBar->SetPercent(Percent);
	}

	if (HPText) {
		HPText->SetText(FText::FromString(FString::FromInt(UpdatedHP)));
	}
}

void UFPSMainWidget::UpdateStaminarHUD(const float UpdatedStaminarPoint)
{
	if (StaminarBar) {
		float Percent = UpdatedStaminarPoint / 100.0f;
		StaminarBar->SetPercent(Percent);
	}
}

void UFPSMainWidget::ChangeGunHUD(int32 WeaponSlot)
{
	EWeaponSlot WidgetWeaponSlot = (EWeaponSlot)WeaponSlot;

	if (WidgetWeaponSlot == EWeaponSlot::Primary) {
		FVector2D WidgetPosition(0.0f, 0.0f);
		FVector2D WidgetScale(1.0f, 1.0f);

		FWidgetTransform RifleWidgetTransform(
			WidgetPosition,
			WidgetScale,
			FVector2D(0.0f, 0.0f),
			FMath::DegreesToRadians(0.0f)
		);
		PrimaryWeaponImage->SetRenderTransform(RifleWidgetTransform);

		WidgetPosition = FVector2D(40.0f,-100.0f);
		WidgetScale = FVector2D(0.5f, 0.5f);

		FWidgetTransform PistolWidgetTransform(
			WidgetPosition,
			WidgetScale,
			FVector2D(0.0f, 0.0f),
			FMath::DegreesToRadians(0.0f)
		);
		SecondaryWeaponImage->SetRenderTransform(PistolWidgetTransform);
	}
	else if (WidgetWeaponSlot == EWeaponSlot::Secondary) {
		FVector2D WidgetPosition(65.0f, -100.0f);
		FVector2D WidgetScale(0.5f, 0.5f);

		FWidgetTransform RifleWidgetTransform(
			WidgetPosition,
			WidgetScale,
			FVector2D(0.0f, 0.0f),
			FMath::DegreesToRadians(0.0f)
		);
		PrimaryWeaponImage->SetRenderTransform(RifleWidgetTransform);
		
		WidgetPosition = FVector2D(0.0f, 0.0f);
		WidgetScale = FVector2D(1.0f, 1.0f);

		FWidgetTransform PistolWidgetTransform(
			WidgetPosition,
			WidgetScale,
			FVector2D(0.0f, 0.0f),
			FMath::DegreesToRadians(0.0f)
		);
		SecondaryWeaponImage->SetRenderTransform(PistolWidgetTransform);
	}
}

void UFPSMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	IPlayerHUDInterface* PlayerPawn = Cast<IPlayerHUDInterface>(GetOwningPlayerPawn());
	if (PlayerPawn) {
		PlayerPawn->SetupMainHUD(this);
	}
}
 