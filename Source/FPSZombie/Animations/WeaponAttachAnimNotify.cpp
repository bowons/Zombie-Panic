// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/WeaponAttachAnimNotify.h"
#include "Interfaces/CharacterAttachmentInterface.h"

void UWeaponAttachAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp) {
		ICharacterAttachmentInterface* Player = Cast<ICharacterAttachmentInterface>(MeshComp->GetOwner());

		if (Player) {
			Player->AttachWeapon(SocketName);
		}
	}
}
