// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/FPSPlayerAniminstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFPSPlayerAniminstance::UFPSPlayerAniminstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
	LandingThreshould = -400.0f;
}

void UFPSPlayerAniminstance::SetCharacterMovementInputVector(FVector2D& MovementVector)
{
	FPSPlayerMovementVector = FMath::Lerp(FPSPlayerMovementVector, MovementVector, 0.1f);
}

void UFPSPlayerAniminstance::SetCharacterLookRotator(FRotator& Rotation)
{
	CameraRotation = Rotation;
}

float UFPSPlayerAniminstance::GetCharacterMovementSpeed()
{
	return GroundSpeed;
}

void UFPSPlayerAniminstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner) {
		Movement = Owner->GetCharacterMovement();
	}
}

void UFPSPlayerAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement) {
		Velocity = Movement->Velocity;

		float NewGroundSpeed = Velocity.Size2D();
		
		if (NewGroundSpeed < MovingThreshould) {
			FVector2D zeroVec = FVector2D(0.0f, 0.0f);
			SetCharacterMovementInputVector(zeroVec);
		}

		// 기존 속도와 새 속도 사이에 보간 (속도가 급격히 변하지 않도록)
		GroundSpeed = FMath::Lerp(GroundSpeed, NewGroundSpeed, 0.05f);  // 0.1f는 보간 속도

		bIsFalling = Movement->IsFalling();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
		
		float JumpVelocity = Velocity.Z;
		uint8 CheckGrounding = (JumpVelocity / FMath::Abs(JumpVelocity)) <= 0 && JumpVelocity <= LandingThreshould;
		
		bIsLanding = CheckGrounding;
	}
}
