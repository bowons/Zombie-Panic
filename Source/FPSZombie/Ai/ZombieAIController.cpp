// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/ZombieAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"

AZombieAIController::AZombieAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> ZombieBlackBoardRef(TEXT("/Script/AIModule.BlackboardData'/Game/FPSZombie/AI/BB_Zombie.BB_Zombie'"));
	if (nullptr != ZombieBlackBoardRef.Object) {
		ZombieBlackBoard = ZombieBlackBoardRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> ZombieBehaviorTreeRef(TEXT("/Script/AIModule.BehaviorTree'/Game/FPSZombie/AI/BT_Zombie.BT_Zombie'"));
	if (nullptr != ZombieBehaviorTreeRef.Object) {
		ZombieBehaviorTree = ZombieBehaviorTreeRef.Object;
	}

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig) {
		SightConfig->SightRadius = 600.0f;
		SightConfig->LoseSightRadius = 1200.0f;
		SightConfig->PeripheralVisionAngleDegrees = 120.0f;
		SightConfig->SetMaxAge(5.0f);

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnTargetPerceptionUpdated);
	}
	
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	if (HearingConfig) {
		HearingConfig->HearingRange = 1500.0f;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AIPerception->ConfigureSense(*HearingConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	}
	
}

void AZombieAIController::SetupBT()
{
	UBlackboardComponent* BBComponent = Blackboard.Get();
	if (UseBlackboard(ZombieBlackBoard, BBComponent)) {
		RunBehaviorTree(ZombieBehaviorTree);
	}

}

void AZombieAIController::SetDead()
{
	UBehaviorTreeComponent* BTCopmonent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTCopmonent) {
		BTCopmonent->StopTree();
	}
}

void AZombieAIController::OnPossess(APawn* AIPawn)
{
	Super::OnPossess(AIPawn);
	SetupBT();
}

FTimerHandle ForgotTimerHandle;

void AZombieAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->ActorHasTag("Player")) return;

	if (Stimulus.Type == SightConfig->GetSenseID())
	{
		
		if (Stimulus.WasSuccessfullySensed())
		{
			GetBlackboardComponent()->SetValueAsObject("Target", Actor);
			GetBlackboardComponent()->SetValueAsBool("HasTarget", true);

			GetWorld()->GetTimerManager().ClearTimer(ForgotTimerHandle);
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(
				ForgotTimerHandle,
				[this]()
				{
					GetBlackboardComponent()->ClearValue("Target");
					GetBlackboardComponent()->SetValueAsBool("HasTarget", false);
				},
				5.0f,
				false
			);
		}
	}
	else if (Stimulus.Type == HearingConfig->GetSenseID())
	{
		if (GetBlackboardComponent()->GetValueAsBool("HasTarget"))
			return;

		if (Stimulus.WasSuccessfullySensed())
		{
			GetBlackboardComponent()->SetValueAsVector("NoiseLocation", Stimulus.StimulusLocation);
			GetBlackboardComponent()->SetValueAsBool("HearNoise", true);
		}
	}
}

