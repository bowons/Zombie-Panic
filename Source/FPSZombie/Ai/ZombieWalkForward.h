// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ZombieWalkForward.generated.h"

/**
 * 
 */
USTRUCT()
struct FWalkForwardTaskMemory
{
	GENERATED_BODY()

	float WalkDuration = 0.0f;
	float StartTime = 0.0f;
};

UCLASS()
class FPSZOMBIE_API UZombieWalkForward : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UZombieWalkForward();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	
	UPROPERTY(EditAnywhere, Category = "Walk")
	float MinWalkTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Walk")
	float MaxWalkTime = 7.5f;

};
