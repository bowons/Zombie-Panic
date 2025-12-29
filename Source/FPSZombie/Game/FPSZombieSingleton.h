// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameData/FPSZombieRoundInfo.h"
#include "FPSZombieSingleton.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UFPSZombieSingleton : public UObject
{
	GENERATED_BODY()

public:
	UFPSZombieSingleton();
	static UFPSZombieSingleton& Get();

public:
	FORCEINLINE FFPSZombieRoundInfo GetRoundInfo(int32 Round) const { return RoundInfoTable.IsValidIndex(Round - 1) ? RoundInfoTable[Round - 1] : FFPSZombieRoundInfo(); }
	
	UPROPERTY()
	int32 MaxRound;

private:
	TArray<FFPSZombieRoundInfo> RoundInfoTable;
};
