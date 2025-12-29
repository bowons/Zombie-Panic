// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPSZombieSingleton.h"

UFPSZombieSingleton::UFPSZombieSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/FPSZombie/GameData/ZombieRoundInfo.ZombieRoundInfo'"));
	
	if (DataTableRef.Succeeded()) {
		const UDataTable* DataTable = DataTableRef.Object;
		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, RoundInfoTable,
			[](uint8* Value) {
				return *reinterpret_cast<FFPSZombieRoundInfo*>(Value);
			}
		);
		MaxRound = RoundInfoTable.Num();
	}

}

UFPSZombieSingleton& UFPSZombieSingleton::Get()
{
	// TODO: insert return statement here
	UFPSZombieSingleton* Singleton = CastChecked<UFPSZombieSingleton>(GEngine->GameSingleton);
	if (Singleton) {
		return *Singleton;
	}

	return *NewObject <UFPSZombieSingleton>();
}
