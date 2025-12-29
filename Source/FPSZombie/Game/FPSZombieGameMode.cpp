// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPSZombieGameMode.h"
#include "Character/FPSPlayerController.h"
#include "Character/FPSCharacterPlayer.h"
#include "Game/FPSZombieRoundManager.h"
#include "Game/FPSZombieSingleton.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

AFPSZombieGameMode::AFPSZombieGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnRef(TEXT("/Script/CoreUObject.Class'/Script/FPSZombie.FPSCharacterPlayer'"));
	if (PlayerPawnRef.Class) {
		DefaultPawnClass = PlayerPawnRef.Class;
	}

	static ConstructorHelpers::FClassFinder<AFPSPlayerController> PlayerControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/FPSZombie/Blueprints/BP_FPSPlayerController.BP_FPSPlayerController_C'"));
	if (PlayerControllerClassRef.Class) {
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UFPSGameWidget> GameWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FPSZombie/Widget/FPSZombie_GameWidget.FPSZombie_GameWidget_C'"));
	if (GameWidgetClassRef.Class) {
		GameModeWidgetClass = GameWidgetClassRef.Class;
	}
}


void AFPSZombieGameMode::UpdateTimer()
{
	RemainingTime -= 1.0f;
	if (RemainingTime <= 0.0f) {
		RemainingTime = 0.0f;
		GetWorldTimerManager().ClearTimer(RoundTimerHandle);
		
		// 게임 오버
		GameOver();
	}
	
	if (GameModeWidget) {
		GameModeWidget->UpdateTimeText(RemainingTime);
	}
	//OnTimeChanged.Broadcast(RemainingTime);
}

void AFPSZombieGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GameModeWidgetClass) 
	{
		UUserWidget* GameWidget = CreateWidget<UUserWidget>(GetWorld(), GameModeWidgetClass);
		if (GameWidget) {
			GameWidget->AddToViewport(10);

			GameModeWidget = CastChecked<UFPSGameWidget>(GameWidget);;
//			GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &AFPSZombieGameMode::UpdateTimer, 1.0f, true);
		}
	}

	RoundManager = GetWorld()->SpawnActor<AFPSZombieRoundManager>(AFPSZombieRoundManager::StaticClass());
	StartRound();
}

void AFPSZombieGameMode::StartRound()
{
	if (CurrentRoundIndex > UFPSZombieSingleton::Get().MaxRound)
	{
		// 게임 클리어 코드
		GameClear();
		return;
	}

	// 라운드 시작 알림
	if (GameModeWidget)
	{
		FString RoundText = FString::Printf(TEXT("Round %d"), CurrentRoundIndex);
		GameModeWidget->NotifyMessage(RoundText);
	}

	RemainingTime = 150.0f;
	GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &AFPSZombieGameMode::UpdateTimer, 1.0f, true);

	if (RoundManager) {
		RoundManager->StartRound(CurrentRoundIndex);

		int32 ZombieCount = RoundManager->GetRemainingZombies();
		if (GameModeWidget)
		{
			GameModeWidget->UpdateRemainingText(ZombieCount);
		}
	}
}

void AFPSZombieGameMode::OnRoundEnd()
{
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);

	if (CurrentRoundIndex + 1 > UFPSZombieSingleton::Get().MaxRound)
	{
		// 게임 클리어 코드
		GameClear();
		return;
	}
	
	if (GameModeWidget)
    {
		GameModeWidget->NotifyMessage(TEXT("Round Complete!"));
        //GameModeWidget->UpdateTimeText(RemainingTime);
    }

	CurrentRoundIndex++;
	OnRoundStarted.Broadcast(CurrentRoundIndex);

	BreakTimeRemaining = 10.0f;
	GetWorldTimerManager().SetTimer(BreakTimerHandle, this, &AFPSZombieGameMode::UpdateBreakTimer, 1.0f, true);
}

void AFPSZombieGameMode::SetAllWidgetsHiddenExcept(UUserWidget* ExceptWidget, bool HiddenFlag)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (Widget && Widget != ExceptWidget)
		{
			Widget->SetVisibility(HiddenFlag ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		}
	}
}

void AFPSZombieGameMode::OnPlayerReady(AFPSCharacterPlayer* ReadyPlayer)
{
	if (RoundManager) {
		int32 CurrentRound = RoundManager->GetCurrentRound();
		ReadyPlayer->SetLevel(CurrentRound);
	}
}

void AFPSZombieGameMode::UpdateBreakTimer()
{
	BreakTimeRemaining -= 1.0f;

	if (GameModeWidget)
	{
		GameModeWidget->UpdateTimeText(BreakTimeRemaining);
	}

	if (BreakTimeRemaining <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(BreakTimerHandle);
		StartRound();
	}
}

void AFPSZombieGameMode::GameClear()
{
	if (bGameCleared) return;
	bGameCleared = true;

	GetWorldTimerManager().ClearAllTimersForObject(this);

	APlayerController* PlayerCon = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerCon)
	{
		/*PlayerCon->SetIgnoreMoveInput(true);
		PlayerCon->SetIgnoreLookInput(true);*/

		if (AFPSCharacterPlayer* Player = Cast<AFPSCharacterPlayer>(PlayerCon->GetPawn()))
		{
			Player->DisableInput(PlayerCon);
			//Player->SetActorEnableCollision(false);
		}
	}

	if (GameModeWidget)
	{
		SetAllWidgetsHiddenExcept(GameModeWidget, true);
		GameModeWidget->ShowGameOver("Victory!");
	}
}

void AFPSZombieGameMode::GameOver()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	APlayerController* PlayerCon = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerCon)
	{
		/*PlayerCon->SetIgnoreMoveInput(true);
		PlayerCon->SetIgnoreLookInput(true);*/

		if (AFPSCharacterPlayer* Player = Cast<AFPSCharacterPlayer>(PlayerCon->GetPawn()))
		{
			Player->DisableInput(PlayerCon);
			//Player->SetActorEnableCollision(false);
		}
	}

	if (GameModeWidget)
	{
		SetAllWidgetsHiddenExcept(GameModeWidget, true);
		GameModeWidget->ShowGameOver("Game Over");
	}
}



void AFPSZombieGameMode::RestartGame()
{
	SetAllWidgetsHiddenExcept(GameModeWidget, false);
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AFPSZombieGameMode::HandleZombieDestroyed()
{
	if (!RoundManager) return;

	// 남은 좀비 수 갱신
	int32 ZombieCount = RoundManager->GetRemainingZombies();

	if (GameModeWidget)
	{
		GameModeWidget->UpdateRemainingText(ZombieCount);
	}

	// 조건: 스폰 완료 + 남은 좀비 수 0 -> 라운드 종료
	if (ZombieCount == 0 && RoundManager->IsSpawningFinished())
	{
		OnRoundEnd();
	}
}
