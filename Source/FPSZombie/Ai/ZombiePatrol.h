// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ZombiePatrol.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UZombiePatrol : public UBTService
{
	GENERATED_BODY()
	
public:
	UZombiePatrol();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Sight")
	float FindNoiseRadius = 350.0f;
};
