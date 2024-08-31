// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSItchIntegrationGameMode.h"
#include "EOSItchIntegrationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEOSItchIntegrationGameMode::AEOSItchIntegrationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
