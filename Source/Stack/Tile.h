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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ReverseDirection();
	bool ShouldReverse() const;

	void DisableMovement();
	void SetColor(FLinearColor && Color);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	class UMaterial* RedTileMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Settings")
	UPhysicalMaterial* PhysicalMaterial;

	UStaticMesh* SM_Mesh;
	
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

	FVector CurrentDestLocation;

private:
	bool bCanMove;
};
