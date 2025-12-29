// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ZombieAttackTarget.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UZombieAttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UZombieAttackTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
