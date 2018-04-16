// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BlockSpawner.generated.h"

UCLASS()
class STACK_API ABlockSpawner : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABlockSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetTileCallback();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnPoints")
	class UArrowComponent* RightSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnPoints")
	class UArrowComponent* LeftSpawnPoint;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* OurCameraSpringArm;

	class UCameraComponent* OurCamera;
	
};
