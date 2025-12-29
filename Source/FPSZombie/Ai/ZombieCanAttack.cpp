// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/ZombieCanAttack.h"
#include "AIController.h"
#include "Character/FPSZombieCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UZombieCanAttack::UZombieCanAttack()
{
	NodeName = TEXT("CanAttack");
}

bool UZombieCanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!AIPawn) {
		return false;
	}

	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Target"));
	if (!Target) {
		return false;
	}

	float DistanceToTarget = AIPawn->GetDistanceTo(Target);
	bResult = (DistanceToTarget <= AttackRange);

	ACharacter* Zombie = Cast<ACharacter>(AIPawn);
	Zombie->GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	return bResult;
}
