// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ZombieCanAttack.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API UZombieCanAttack : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UZombieCanAttack();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	float AttackRange = 150.0f;
};
