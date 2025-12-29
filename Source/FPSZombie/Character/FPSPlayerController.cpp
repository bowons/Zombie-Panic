// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPSPlayerController.h"
#include "WIdget/CrossHairWidget.h"
#include "WIdget/FPSMainWidget.h"

AFPSPlayerController::AFPSPlayerController()
{
	static ConstructorHelpers::FClassFinder<UFPSMainWidget> MainHUDWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FPSZombie/Widget/FPSZombie_Status_UI.FPSZombie_Status_UI_C'"));

	if (MainHUDWidgetRef.Class) {
		MainHUDWidgetClass = MainHUDWidgetRef.Class;
	}
	
}

void AFPSPlayerController::EnableUIMode(UUserWidget* FocusWidget)
{
	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AFPSPlayerController::EnableGameMode()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (MainHUDWidgetClass) {
		UFPSMainWidget* MainHUD = CreateWidget<UFPSMainWidget>(this, MainHUDWidgetClass);

		if (MainHUD) {
			MainHUD->AddToViewport();
		}
	}

}
