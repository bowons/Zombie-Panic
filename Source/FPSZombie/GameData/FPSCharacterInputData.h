// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSCharacterInputData.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UFPSCharacterInputData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UFPSCharacterInputData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TMap<FName, TObjectPtr<class UInputAction>> CharacterActions;
};
