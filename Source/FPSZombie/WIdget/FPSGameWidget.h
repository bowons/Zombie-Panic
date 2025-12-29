// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UFPSGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* MainPanel;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Clock;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Remaining;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* NotifyPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NotifyText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* RestartPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* GameResultText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* RestartButton;

public:
	UFUNCTION()
	void UpdateTimeText(float NewTime);

	UFUNCTION()
	void UpdateRemainingText(int32 RemainingCount);

	UFUNCTION()
	void NotifyMessage(FString Message);

	UFUNCTION()
	void ShowGameOver(FString Message);
	
	UFUNCTION()
	void ShowMainPanel();

	UFUNCTION()
	void OnRestartButtonClicked();

protected:
	virtual void NativeConstruct() override;

};
