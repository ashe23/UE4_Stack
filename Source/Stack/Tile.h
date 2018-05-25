// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class STACK_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	// This part for Tile Color Changing
	UPROPERTY()
	class UTimelineComponent* TileColorChangeTimeline;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class UMaterial* DefaultMaterial;

	UStaticMesh* SM_Mesh;
protected:

	UFUNCTION()
	void TimelineCallback(FLinearColor Color);

	UFUNCTION()
	void TimelineFinishedCallback();

	void PlayTimeline();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ReverseDirection();
	bool ShouldReverse() const;

	void DisableMovement();
	void SetColor(FLinearColor Color);

	FVector CurrentDestLocation;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	class UMaterialInterface* TileMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	float ReverseDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	float Speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	FVector MoveDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	FVector StartPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	FVector EndPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	bool bCanMove;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	class UCurveLinearColor* ColorChangeCurve;

	float CurrentPlaybackPosition;
};
