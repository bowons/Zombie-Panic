// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPSZombieRoundManager.h"
#include "Character/FPSZombieCharacter.h"
#include "Character/FPSCharacterPlayer.h"
#include "Game/FPSZombieSingleton.h"
#include "GameData/FPSZombieRoundInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FPSZombieGameMode.h"

// Sets default values
AFPSZombieRoundManager::AFPSZombieRoundManager()
{
	static ConstructorHelpers::FClassFinder<AFPSZombieCharacter> ZombieBPRef(TEXT("/Script/Engine.Blueprint'/Game/FPSZombie/Blueprints/BP_Zombie.BP_Zombie_C'"));

	if (ZombieBPRef.Succeeded()) {
		ZombieClass = ZombieBPRef.Class;
	}

}

// Called when the game starts or when spawned
void AFPSZombieRoundManager::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("ZombieSpawn"), SpawnPoints);
}

void AFPSZombieRoundManager::TryZombieSpawn()
{
	if (SpawnCount == CurrentRoundZombieCount) {
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	if (ExistingZombies >= MaxZombiesOnMap)
		return; // 좀비가 너무 많아서 대기

	SpawnZombie();
}

void AFPSZombieRoundManager::OnZombieDestroyed(AActor* DestroyedActor)
{
	ExistingZombies = FMath::Clamp(ExistingZombies - 1, 0, CurrentRoundZombieCount);
	RemainingZombies = FMath::Clamp(RemainingZombies - 1, 0, CurrentRoundZombieCount);

	if (AFPSZombieGameMode* GameMode = Cast<AFPSZombieGameMode>(UGameplayStatics::GetGameMode(this))) {
		GameMode->HandleZombieDestroyed();
	}
}

void AFPSZombieRoundManager::StartRound(int32 RoundIndex)
{
	CurrentRound = RoundIndex;
	
	const FFPSZombieRoundInfo RoundInfo = UFPSZombieSingleton::Get().GetRoundInfo(CurrentRound);
	CurrentRoundZombieCount = RoundInfo.ZombieCount;
	
	RemainingZombies = RoundInfo.ZombieCount;
	ExistingZombies = 0;
	SpawnCount = 0;

	/*if (AFPSZombieGameMode* GameMode = Cast<AFPSZombieGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnRoundStarted.Broadcast(RoundIndex);
	}*/

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AFPSZombieRoundManager::TryZombieSpawn, 2.0f, true);
}

void AFPSZombieRoundManager::SpawnZombie()
{
	if (SpawnPoints.Num() == 0 || !ZombieClass) return;

	int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	FVector Location = SpawnPoints[RandomIndex]->GetActorLocation();
	FRotator Rotation = SpawnPoints[RandomIndex]->GetActorRotation();

	FTransform SpawnTransform(Rotation, Location);

	AFPSZombieCharacter* Zombie = GetWorld()->SpawnActorDeferred<AFPSZombieCharacter>(ZombieClass, SpawnTransform);
	if (Zombie) 
	{
		Zombie->OnDestroyed.AddDynamic(this, &AFPSZombieRoundManager::OnZombieDestroyed);
		Zombie->SetLevel(CurrentRound);
		Zombie->FinishSpawning(SpawnTransform);

		ExistingZombies++; // 현재 좀비 수는 늘림
		SpawnCount++;
	}
}