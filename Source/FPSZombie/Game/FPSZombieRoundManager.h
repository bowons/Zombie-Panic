// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSZombieRoundManager.generated.h"

UCLASS()
class FPSZOMBIE_API AFPSZombieRoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSZombieRoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TryZombieSpawn();

	UFUNCTION()
	void OnZombieDestroyed(AActor* DestroyedActor);

public:
	void StartRound(int32 RoundIndex);
	void SpawnZombie();
	FORCEINLINE int32 GetCurrentRoundZombieCount() const { return CurrentRoundZombieCount; }
	FORCEINLINE int32 GetCurrentRound() const { return CurrentRound; }
	FORCEINLINE int32 GetRemainingZombies() const { return RemainingZombies; };
	FORCEINLINE int32 GetExistingZombies() const { return ExistingZombies; };
	FORCEINLINE	bool IsSpawningFinished() const { return RemainingZombies <= 0; };

protected:
	UPROPERTY()
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Zombie")
	TSubclassOf<class AFPSZombieCharacter> ZombieClass;

	
	int32 CurrentRoundZombieCount = 0;
	
	int32 SpawnCount = 0;
	int32 RemainingZombies = 0;
	int32 ExistingZombies = 0;

	int32 CurrentRound = 1;

	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Zombie")
	int32 MaxZombiesOnMap = 7;
};
