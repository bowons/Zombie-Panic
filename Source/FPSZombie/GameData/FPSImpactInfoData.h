// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSImpactInfoData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FImpactInfo {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactEffect)
	TObjectPtr<class UNiagaraSystem> ImpactDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSound)
	TObjectPtr<class USoundBase> ImpactSound;
};

UCLASS()
class FPSZOMBIE_API UFPSImpactInfoData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UFPSImpactInfoData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ImpactInfo)
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactInfo> ImpactInfoMap;
};