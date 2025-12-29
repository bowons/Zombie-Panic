// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WIdget/FPSGameWidget.h"
#include "FPSZombieGameMode.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStartedSignature, int32, RoundIndex);

UCLASS()
class FPSZOMBIE_API AFPSZombieGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSZombieGameMode();
	
protected:
	UFUNCTION()
	void UpdateTimer();
	virtual void BeginPlay() override;
	
	void StartRound();
	void OnRoundEnd();

	UFUNCTION(BlueprintCallable)
	void SetAllWidgetsHiddenExcept(UUserWidget* ExceptWidget, bool HiddenFlag);

public:
	UPROPERTY(BlueprintAssignable, Category = "Round")
	FOnRoundStartedSignature OnRoundStarted;

	void GameClear();
	void GameOver();
	void RestartGame();
	void HandleZombieDestroyed();
	void OnPlayerReady(class AFPSCharacterPlayer* ReadyPlayer);

protected:
	FTimerHandle RoundTimerHandle;
	float RemainingTime = 150.0f;

	FTimerHandle BreakTimerHandle;
	float BreakTimeRemaining = 10.0f;
	void UpdateBreakTimer();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameModeWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UFPSGameWidget* GameModeWidget;

	UPROPERTY()
	TObjectPtr<class AFPSZombieRoundManager> RoundManager;

	int32 CurrentRoundIndex = 1;

	bool bGameCleared = false;
};
