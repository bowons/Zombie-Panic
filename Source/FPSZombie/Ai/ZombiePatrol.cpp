// Fill out your copyright notice in the Description page of Project Settings.

#include "Ai/ZombiePatrol.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UZombiePatrol::UZombiePatrol()
{
	bNotifyTick = true;
}

void UZombiePatrol::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!AIPawn) return;

	const FVector CurrentLocation = AIPawn->GetActorLocation();
	const FVector NoiseLocation = BlackboardComp->GetValueAsVector("NoiseLocation");

	const float DistanceToNoise = FVector::Dist(CurrentLocation, NoiseLocation);

	if (DistanceToNoise <= FindNoiseRadius)
	{
		BlackboardComp->ClearValue("NoiseLocation");
		BlackboardComp->SetValueAsBool("HearNoise", false);
	}
}
