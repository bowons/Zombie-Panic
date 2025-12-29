// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/WeaponSwapAnimNotify.h"
#include "Interfaces/CharacterAttachmentInterface.h"

void UWeaponSwapAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp) {
		ICharacterAttachmentInterface* Player = Cast<ICharacterAttachmentInterface>(MeshComp->GetOwner());

		if (Player) {
			Player->SwapWeapon(WeaponType);
		}
	}
}
