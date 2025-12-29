// Fill out your copyright notice in the Description page of Project Settings.

#include "Ai/ZombieAttackTarget.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/FPSZombieCharacter.h"
#include "Animation/AnimInstance.h"

UZombieAttackTarget::UZombieAttackTarget()
{
    bNotifyTick = false;
    bCreateNodeInstance = true; // 다수 AI의 델리게이트 처리를 위해
}

EBTNodeResult::Type UZombieAttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return EBTNodeResult::Failed;

    ACharacter* Zombie = Cast<ACharacter>(AIPawn);
    if (!Zombie) return EBTNodeResult::Failed;

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
    if (!Target) return EBTNodeResult::Failed;

    AFPSZombieCharacter* ZombieChar = Cast<AFPSZombieCharacter>(Zombie);
    if (!ZombieChar) if (!Target) return EBTNodeResult::Failed;
    
    UBehaviorTreeComponent* LocalOwnerComp = &OwnerComp;

    FOnMontageEnded EndDelegate;
    EndDelegate.BindLambda([this, ZombieChar, LocalOwnerComp](UAnimMontage* Montage, bool bInterrupted)
    {
        FinishLatentTask(*LocalOwnerComp, EBTNodeResult::Succeeded);
        ZombieChar->ClearAttackDelegate();
    });
    
    ZombieChar->PlayAttackMontage();
    ZombieChar->SetAttackEndDelegate(EndDelegate);

    return EBTNodeResult::InProgress;
}