// Fill out your copyright notice in the Description page of Project Settings.

#include "StackGameModeBase.h"
#include "BlockSpawner.h"

AStackGameModeBase::AStackGameModeBase()
{
	DefaultPawnClass = ABlockSpawner::StaticClass();


	CurrentTileCount = 0;
}
