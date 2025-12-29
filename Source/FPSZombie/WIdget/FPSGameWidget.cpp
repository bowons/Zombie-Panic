// Fill out your copyright notice in the Description page of Project Settings.


#include "WIdget/FPSGameWidget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Game/FPSZombieGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/WidgetControlModeInterface.h"

void UFPSGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UFPSGameWidget::OnRestartButtonClicked);
	}
}

void UFPSGameWidget::UpdateTimeText(float NewTime)
{
    int32 Minutes = FMath::FloorToInt(NewTime / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(NewTime, 60.0f));

    FString TimeString = FString::Printf(TEXT("%d : %02d"), Minutes, Seconds);
    Clock->SetText(FText::FromString(TimeString));
}

void UFPSGameWidget::UpdateRemainingText(int32 RemainingCount)
{
    FString CountText = FString::Printf(TEXT("Remaining : %d"), RemainingCount);
    Remaining->SetText(FText::FromString(CountText));
}

void UFPSGameWidget::NotifyMessage(FString Message)
{
	if (!NotifyText || !MainPanel || !RestartPanel || !NotifyPanel) return;

	MainPanel->SetVisibility(ESlateVisibility::Hidden);
	RestartPanel->SetVisibility(ESlateVisibility::Hidden);
	NotifyPanel->SetVisibility(ESlateVisibility::Visible);
	NotifyText->SetText(FText::FromString(Message));
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			MainPanel->SetVisibility(ESlateVisibility::Visible);
			RestartPanel->SetVisibility(ESlateVisibility::Hidden);
			NotifyPanel->SetVisibility(ESlateVisibility::Hidden);
		}, 1.0f, false);
}

void UFPSGameWidget::ShowGameOver(FString Message)
{
	if (MainPanel) MainPanel->SetVisibility(ESlateVisibility::Hidden);
	if (NotifyPanel) NotifyPanel->SetVisibility(ESlateVisibility::Hidden);
	if (RestartPanel) RestartPanel->SetVisibility(ESlateVisibility::Visible);

	if (GameResultText)
	{
		GameResultText->SetText(FText::FromString(Message));
	}


	IWidgetControlModeInterface* FPSPlayerController = Cast<IWidgetControlModeInterface>(GetWorld()->GetFirstPlayerController());

	if (FPSPlayerController) {
		FPSPlayerController->EnableUIMode(this);
	}
}
	
void UFPSGameWidget::ShowMainPanel()
{
	if (MainPanel) MainPanel->SetVisibility(ESlateVisibility::Visible);
	if (NotifyPanel) NotifyPanel->SetVisibility(ESlateVisibility::Hidden);
	if (RestartPanel) RestartPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UFPSGameWidget::OnRestartButtonClicked()
{
	if (AFPSZombieGameMode* GameMode = Cast<AFPSZombieGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		IWidgetControlModeInterface* FPSPlayerController = Cast<IWidgetControlModeInterface>(GetWorld()->GetFirstPlayerController());

		if (FPSPlayerController) {
			FPSPlayerController->EnableGameMode();
		}
		GameMode->RestartGame();
	}
}
