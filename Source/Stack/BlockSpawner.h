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
	
	void SpawnInitialTiles();

	void SetTileCallback();

	void CalcTilesIntersection();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Pawn Settings")
	void SpawnTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnPoints")
	class UArrowComponent* RightSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnPoints")
	class UArrowComponent* LeftSpawnPoint;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* OurCameraSpringArm;

	class UCameraComponent* OurCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn Settings")
	float ZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn Settings")
	bool bIsRightTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn Settings")
	class ATile* CurrentTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn Settings")
	class ATile* PreviousTile;
private:
	void SetCurrentTileLocation();
	void SetCurrentTileScale();
	void UpdateArrowLocations();
	bool IsGameOver() const;
	void GenerateExtraTilePart();


	FVector SpawnScale;
	FVector NewCenter;
	FVector ExtraPartCenter;
	float ExtraPartScale;
};
