// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ZombieTurnDirection.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UZombieTurnDirection : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UZombieTurnDirection();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Turn Settings")
	float TurnSpeed = 180.0f; // degrees per second

	UPROPERTY(EditAnywhere, Category = "Turn Settings")
	float MinTurnAngle = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Turn Settings")
	float MaxTurnAngle = 180.0f;

};

USTRUCT()
struct FTurnRandomDirectionTaskMemory
{
	GENERATED_BODY()

	FRotator TargetRotation;
	bool bHasTarget = false;
};