// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AZombieAIController();

	void SetupBT();
	void SetDead();

protected:
	virtual void OnPossess(APawn* AIPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> ZombieBlackBoard;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> ZombieBehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UPROPERTY()
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
