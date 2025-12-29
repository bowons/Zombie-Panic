// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapons/FPSWeaponBase.h"
#include "CharacterAttachmentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterAttachmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPSZOMBIE_API ICharacterAttachmentInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AttachWeapon(FName SocketName) = 0;
	virtual void SwapWeapon(EWeaponSlot WeaponType) = 0;
};
