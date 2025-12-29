// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/ZombieAttackNotify.h"
#include "Interfaces/ZombieAttackInterface.h"

void UZombieAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp) {
		IZombieAttackInterface* Zombie = Cast<IZombieAttackInterface>(MeshComp->GetOwner());

		if (Zombie) {
			Zombie->Attack();
		}
	}
}
