// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StackGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STACK_API AStackGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AStackGameModeBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Stats")
	int32 CurrentTileCount;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Stats")
	bool IsGameOver;
	
};
