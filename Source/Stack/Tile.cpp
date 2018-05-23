// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "TimerManager.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Speed = 300.0f;
	ReverseDistance = 500.0f;
	bCanMove = true;

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> SM_Mesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> TileMaterial;
		FConstructorStatics() :
			SM_Mesh(TEXT("StaticMesh'/Game/Assets/Meshes/Tile.Tile'")),
			TileMaterial(TEXT("MaterialInstanceConstant'/Game/Assets/Materials/MI_Tile.MI_Tile'"))
		{
		}
	};

	static FConstructorStatics ConstructorStatics;

	TileMaterial = ConstructorStatics.TileMaterial.Get();

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);
	TileMesh->SetStaticMesh(ConstructorStatics.SM_Mesh.Get());
	TileMesh->SetMobility(EComponentMobility::Movable);
	TileMesh->SetAngularDamping(0.3f);
	TileMesh->SetLinearDamping(0.4f);
	TileMesh->SetVisibility(true);
	TileMesh->CastShadow = false;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();	


	// Every 2 sec change color of tile
	FTimerHandle EmptyTimehandler;
	GetWorldTimerManager().SetTimer(EmptyTimehandler, this, &ATile::SetColor, 2.0f, true);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanMove)
	{
		// Moving platform every second
		if (ShouldReverse())
		{
			ReverseDirection();
		}

		FVector CurrentLocation = GetActorLocation();
		FVector Direction = CurrentDestLocation - CurrentLocation;
		Direction.Normalize();

		CurrentLocation += Speed * DeltaTime * Direction;
		SetActorLocation(CurrentLocation, false, nullptr, ETeleportType::None);
	}
}

void ATile::ReverseDirection()
{
	CurrentDestLocation = (CurrentDestLocation == StartPosition) ? EndPosition : StartPosition;
}

bool ATile::ShouldReverse() const
{
	if (CurrentDestLocation == StartPosition)
	{
		return FVector::Dist(EndPosition, GetActorLocation()) >= ReverseDistance;
	}
	else
	{
		return FVector::Dist(StartPosition, GetActorLocation()) >= ReverseDistance;
	}
}

void ATile::DisableMovement()
{
	bCanMove = false;
}

void ATile::SetColor(FLinearColor && Color)
{
	UMaterialInstanceDynamic* MIBaseColor = UMaterialInstanceDynamic::Create(TileMesh->GetMaterial(0), this);

	MIBaseColor->SetVectorParameterValue(FName(TEXT("BaseColor")), Color);

	TileMesh->SetMaterial(0, MIBaseColor);
}

