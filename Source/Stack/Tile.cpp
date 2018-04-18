// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Speed = 300.0f;
	ReverseDistance = 700.0f;

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> SM_Mesh;
		FConstructorStatics() :
			SM_Mesh(TEXT("StaticMesh'/Game/Assets/Tile.Tile'"))
		{
		}
	};

	static FConstructorStatics ConstructorStatics;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);
	TileMesh->SetStaticMesh(ConstructorStatics.SM_Mesh.Get());
	TileMesh->SetMobility(EComponentMobility::Movable);
	TileMesh->SetVisibility(true);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
