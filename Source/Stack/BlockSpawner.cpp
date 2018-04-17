// Fill out your copyright notice in the Description page of Project Settings.

#include "BlockSpawner.h"
#include "Tile.h"

#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Public/Rendering/PositionVertexBuffer.h"
#include "Engine/World.h"
#include "Engine/Engine.h"


// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ZOffset = 10.0f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Setting default spawn points (Arrow components)
	RightSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RightSpawnPoint"));
	RightSpawnPoint->SetRelativeLocation(FVector{0, -500.0f, 0});
	RightSpawnPoint->SetRelativeRotation(FRotator{0, 90.0f, 0 });

	LeftSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftSpawnPoint"));
	LeftSpawnPoint->SetRelativeLocation(FVector{500.0f, 0, 0});
	LeftSpawnPoint->SetRelativeRotation(FRotator{0, 180.0f, 0.0f});

	RightSpawnPoint->SetupAttachment(RootComponent);
	LeftSpawnPoint->SetupAttachment(RootComponent);

	RightSpawnPoint->bHiddenInGame = false;
	LeftSpawnPoint->bHiddenInGame = false;

	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	OurCameraSpringArm->TargetArmLength = 400.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;
	OurCameraSpringArm->bDoCollisionTest = false;
	OurCameraSpringArm->SetRelativeRotation(FRotator{-30.0f, -45.0f, 0.0f});

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABlockSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Spawning One Tile for Prev
	auto World = GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant find World!"));
		return;
	}

	FTransform Trans{ FVector{0, 0, -10.0f} };
	PreviousTile = World->SpawnActor<ATile>(ATile::StaticClass(), Trans);
	PreviousTile->Speed = 0;


	SpawnTile();
	
}

void ABlockSpawner::SetTileCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("Setting tile callback!"));

	// Moving our Pawn Root Component Up By Zoffset
	AddActorWorldOffset(FVector{ 0, 0, ZOffset });

	// Calculate Tile Scale
	CalcTilesIntersection();
	//CurrentTile->SetActorScale3D(FVector{ 0.5f, 0.5f, 1.0f });

	

	// Spawning New Tile with old scales
	SpawnTile();
}

void ABlockSpawner::CalcTilesIntersection()
{
	// Get current tile location
	// Get previous tile location
	FVector NewCenter = PreviousTile->GetActorLocation() - CurrentTile->GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("New Center: %s"), *NewCenter.ToString());
}

// Called every frame
void ABlockSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABlockSpawner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("SetTile", IE_Pressed, this, &ABlockSpawner::SetTileCallback);
}

void ABlockSpawner::SpawnTile()
{	
	if (CurrentTile && CurrentTile->IsValidLowLevel())
	{
		CurrentTile->Speed = 0;
		PreviousTile = CurrentTile;
	}

	auto World = GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant find World!"));
		return;
	}

	if (bIsRightTurn)
	{
		CurrentTile = World->SpawnActor<ATile>(ATile::StaticClass(), RightSpawnPoint->GetComponentTransform());
		if (CurrentTile)
		{
			CurrentTile->MoveDirection = FVector{ 0, 1.0f, 0 };
			CurrentTile->StartPosition = RightSpawnPoint->GetComponentTransform().GetLocation();
			CurrentTile->EndPosition = CurrentTile->StartPosition + CurrentTile->MoveDirection * CurrentTile->ReverseDistance;
			CurrentTile->CurrentDestLocation = CurrentTile->EndPosition;
		}
	}
	else
	{
		CurrentTile = World->SpawnActor<ATile>(ATile::StaticClass(), LeftSpawnPoint->GetComponentTransform());
		if (CurrentTile)
		{
			CurrentTile->MoveDirection = FVector{ -1.0f, 0, 0 };
			CurrentTile->StartPosition = LeftSpawnPoint->GetComponentTransform().GetLocation();
			CurrentTile->EndPosition = CurrentTile->StartPosition + CurrentTile->MoveDirection * CurrentTile->ReverseDistance;
			CurrentTile->CurrentDestLocation = CurrentTile->EndPosition;
		}
	}

	bIsRightTurn = !bIsRightTurn;
}

