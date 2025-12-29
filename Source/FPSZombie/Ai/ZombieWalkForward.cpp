// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/ZombieWalkForward.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UZombieWalkForward::UZombieWalkForward()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UZombieWalkForward::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FWalkForwardTaskMemory* MyMemory = (FWalkForwardTaskMemory*)NodeMemory;

    MyMemory->WalkDuration = FMath::FRandRange(MinWalkTime, MaxWalkTime);
    MyMemory->StartTime = OwnerComp.GetWorld()->GetTimeSeconds();

	return EBTNodeResult::InProgress;
}

void UZombieWalkForward::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
    ACharacter* Character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Character)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }


    if (Character && Character->GetCharacterMovement())
    {
        FVector ForwardDirection = Character->GetActorForwardVector();

        Character->GetCharacterMovement()->MaxWalkSpeed = 125.0f;
        Character->AddMovementInput(ForwardDirection, 1.0f);

        FWalkForwardTaskMemory* MyMemory = (FWalkForwardTaskMemory*)NodeMemory;
        float CurrentTime = OwnerComp.GetWorld()->GetTimeSeconds();

        // 시간 초과 검사
        if (CurrentTime - MyMemory->StartTime >= MyMemory->WalkDuration)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }

        FVector Start = Character->GetActorLocation();
        FVector End = Start + ForwardDirection * 65.0f;

        FHitResult HitResult;
        FCollisionQueryParams WallParams;
        WallParams.AddIgnoredActor(Character);

        bool bHit = Character->GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Start,
            End,
            ECC_Visibility,
            WallParams
        );

        DrawDebugLine(Character->GetWorld(), Start, End, FColor::Red, false, 0.1f);

        if (bHit)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }
    else
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
}

uint16 UZombieWalkForward::GetInstanceMemorySize() const
{
    return sizeof(FWalkForwardTaskMemory);
}