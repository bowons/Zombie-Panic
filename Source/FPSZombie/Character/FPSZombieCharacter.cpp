// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/FPSZombieCharacter.h"
#include "Engine/DamageEvents.h"
#include "Ai/ZombieAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/FPSZombieSingleton.h"
#include "Character/FPSCharacterPlayer.h"
#include "GameData/FPSZombieRoundInfo.h"

USkeletalMesh* AFPSZombieCharacter::ZombieMesh = nullptr;

AFPSZombieCharacter::AFPSZombieCharacter()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ZombieMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/FPSZombie/Characters/Zombie/Models/Zombie.Zombie'"));

	if (ZombieMeshRef.Succeeded()) {
		ZombieMesh = ZombieMeshRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackAnimMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/FPSZombie/Characters/Zombie/Animations/ZombieAttackMontage.ZombieAttackMontage'"));
	
	if (AttackAnimMontageRef.Succeeded()) {
		AttackMontage = AttackAnimMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadAnimMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/FPSZombie/Characters/Zombie/Animations/ZombieDeadMontage.ZombieDeadMontage'"));

	if (DeadAnimMontageRef.Succeeded()) {
		DeadMontage = DeadAnimMontageRef.Object;
	}

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AFPSZombieCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ZombieMesh) {
		GetMesh()->SetSkeletalMesh(ZombieMesh);
	}
	
	//GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, 0.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(1.1f, 1.1f, 1.1f));
}

void AFPSZombieCharacter::SetLevel(int32 Round)
{
	int CurrentRound = FMath::Clamp(Round, 1, UFPSZombieSingleton::Get().MaxRound);

	FFPSZombieRoundInfo RoundInfo = UFPSZombieSingleton::Get().GetRoundInfo(CurrentRound);
	HP = RoundInfo.ZombieHP;
	
	UpdateHP();
}

float AFPSZombieCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);

		if (0 == (PointDamageEvent->HitInfo.BoneName).Compare(FName(TEXT("Head"))))
		{

			Damage *= 2;
		}

	}

	if (HP >= 0) {
		HP -= Damage;
		UpdateHP();

		if(HP <= 0)
			SetDead();
	}
	
	return Damage;
}

void AFPSZombieCharacter::Attack()
{
	TArray<FHitResult> OutHitResults;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(MeleeAttack), true, this);

	const FVector Start = GetActorLocation() +
		GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bHit)
	{
		for (const FHitResult& Hit : OutHitResults)
		{
			AFPSCharacterPlayer* HitPlayer = Cast<AFPSCharacterPlayer>(Hit.GetActor());
			if (HitPlayer)
			{
				FDamageEvent DamageEvent;
				HitPlayer->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
				break; // 여러 명 맞게 하려면 break 제거
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
#endif
}

void AFPSZombieCharacter::PlayAttackMontage()
{
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
}

void AFPSZombieCharacter::SetAttackEndDelegate(FOnMontageEnded& EndDelegate)
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
}

void AFPSZombieCharacter::ClearAttackDelegate()
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		FOnMontageEnded EmptyDelegate;
		Anim->Montage_SetEndDelegate(EmptyDelegate, AttackMontage);
	}
}

void AFPSZombieCharacter::SetDead()
{
	Super::SetDead();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
	
	DisableHPBar();

	AZombieAIController* ZombieController = Cast<AZombieAIController>(GetController());
	if (ZombieController) {
		ZombieController->SetDead();
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() {
			Destroy();
		}
	), 3.0f, false);
}