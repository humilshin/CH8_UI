// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"
#include "BaseGameState.h"
#include "CH8_UI/CH8_UICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ABaseGameMode::ABaseGameMode()
{
	DefaultPawnClass = nullptr;
}

