// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/WidgetControlModeInterface.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API AFPSPlayerController : public APlayerController, public IWidgetControlModeInterface
{
	GENERATED_BODY()
	
public:
	AFPSPlayerController();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeWeapon();

	UFUNCTION(BlueprintImplementableEvent)
	void ShakeCamera();

	UFUNCTION(BlueprintImplementableEvent)
	void FiringCamShake();

	virtual void EnableUIMode(UUserWidget* FocusWidget) override;
	virtual void EnableGameMode() override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	TSubclassOf<UUserWidget> CrossHairWidgetClass;
};
