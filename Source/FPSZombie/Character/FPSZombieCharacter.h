// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FPSCharacterBase.h"
#include "Interfaces/ZombieAttackInterface.h"
#include "FPSZombieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FPSZOMBIE_API AFPSZombieCharacter : public AFPSCharacterBase, public IZombieAttackInterface
{
	GENERATED_BODY()
	
public:
	AFPSZombieCharacter();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHP();

	UFUNCTION(BlueprintImplementableEvent)
	void DisableHPBar();

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Attack() override;
	virtual void SetLevel(int32 Round) override;
	
	void PlayAttackMontage();
	void SetAttackEndDelegate(FOnMontageEnded& EndDelegate);
	void ClearAttackDelegate();
	
protected:
	virtual void PostInitializeComponents() override;
	virtual void SetDead() override;

	UPROPERTY(VisibleAnywhere, Category="Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, Category = "Dead")
	UAnimMontage* DeadMontage;

private:
	static USkeletalMesh* ZombieMesh;

	const float AttackRadius = 75.0f;
	const float AttackRange = 50.0f;
	const float AttackDamage = 20.0f;
};
