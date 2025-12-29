// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FPSZombieRoundInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FFPSZombieRoundInfo : public FTableRowBase {
	GENERATED_BODY()

public:
	FFPSZombieRoundInfo() : ZombieHP(0.0f), ZombieCount(0), PrimaryAmmo(0), SecondaryAmmo(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Round)
	float ZombieHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Round)
	uint8 ZombieCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Round)
	uint8 PrimaryAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Round)
	uint8 SecondaryAmmo;
};