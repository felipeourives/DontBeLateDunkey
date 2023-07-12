// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunningLatePhilipsGameMode.h"
#include "RunningLatePhilipsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARunningLatePhilipsGameMode::ARunningLatePhilipsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/RunningLatePhilips/Blueprints/BP_DunkieThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
