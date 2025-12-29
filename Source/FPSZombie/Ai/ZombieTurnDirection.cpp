// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ai/ZombieTurnDirection.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UZombieTurnDirection::UZombieTurnDirection()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UZombieTurnDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FTurnRandomDirectionTaskMemory* MyMemory = (FTurnRandomDirectionTaskMemory*)NodeMemory;

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;

    if (!AIPawn)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject("Target"));

    if (Target)
    {
        FVector LookVector = Target->GetActorLocation() - AIPawn->GetActorLocation();
        LookVector.Z = 0.0f;

        MyMemory->TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
        MyMemory->bHasTarget = true;

        TurnSpeed = 360.0f; 

        return EBTNodeResult::InProgress;
    }
    else
    {
        // Target이 없을 경우: 랜덤 방향 회전
        float RandomAngle = FMath::FRandRange(MinTurnAngle, MaxTurnAngle);
        int32 TurnDirection = (FMath::RandBool() ? 1 : -1);
        float TargetYaw = AIPawn->GetActorRotation().Yaw + (RandomAngle * TurnDirection);

        MyMemory->TargetRotation = FRotator(0.0f, TargetYaw, 0.0f);
        MyMemory->bHasTarget = false;

        TurnSpeed = 180.0f; 

        return EBTNodeResult::InProgress;
    }
}

void UZombieTurnDirection::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FTurnRandomDirectionTaskMemory* MyMemory = (FTurnRandomDirectionTaskMemory*)NodeMemory;
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;

    if (!AIPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    FRotator CurrentRotation = AIPawn->GetActorRotation();
    FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, MyMemory->TargetRotation, DeltaSeconds, TurnSpeed);
    AIPawn->SetActorRotation(NewRotation);

    float YawDiff = FMath::Abs(FRotator::NormalizeAxis(NewRotation.Yaw - MyMemory->TargetRotation.Yaw));

    
    if (YawDiff < 2.0f)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

uint16 UZombieTurnDirection::GetInstanceMemorySize() const
{
	return sizeof(FTurnRandomDirectionTaskMemory);
}