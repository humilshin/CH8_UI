// Copyright Epic Games, Inc. All Rights Reserved.

#include "CH8_UIGameMode.h"
#include "CH8_UICharacter.h"
#include "UObject/ConstructorHelpers.h"

ACH8_UIGameMode::ACH8_UIGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
