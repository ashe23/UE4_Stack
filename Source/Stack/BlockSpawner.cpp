// Fill out your copyright notice in the Description page of Project Settings.

#include "BlockSpawner.h"
#include "Tile.h"

#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Public/Rendering/PositionVertexBuffer.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/Engine.h"


// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ZOffset = 10.0f;
	SpawnScale = FVector{ 1.f };

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Setting default spawn points (Arrow components)
	RightSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RightSpawnPoint"));
	RightSpawnPoint->SetRelativeLocation(FVector{ 500.0f, 0, 0 });
	RightSpawnPoint->SetRelativeRotation(FRotator{ 0, 180.f, 0 });

	LeftSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftSpawnPoint"));
	LeftSpawnPoint->SetRelativeLocation(FVector{0,-500.0f, 0});
	LeftSpawnPoint->SetRelativeRotation(FRotator{0, 90.f, 0});

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

	FTransform FirstTileTransform{ FVector{0, 0, -10.0f} };
	PreviousTile = World->SpawnActor<ATile>(ATile::StaticClass(), FirstTileTransform);
	PreviousTile->Speed = 0;


	SpawnTile();	
}

void ABlockSpawner::SetTileCallback()
{		
	// Stopping current tile movement
	CurrentTile->Speed = 0;
	
	// checking if game is over
	if (IsGameOver())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, TEXT("Game Over!!"));
		}
		UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
		return;
	}

	// else

	// Moving our Pawn Root Component Up By Zoffset
	AddActorWorldOffset(FVector{ 0, 0, ZOffset });

	// Calculate Tile Scale
	SetCurrentTileLocation();
	SetCurrentTileScale();

	PreviousTile = CurrentTile;

	bIsRightTurn = !bIsRightTurn;

	UpdateArrowLocations();

	// Spawning New Tile with old scales
	SpawnTile();

}

void ABlockSpawner::CalcTilesIntersection()
{
	

	
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
	auto World = GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant find World!"));
		return;
	}

	// should spawn based on previuos tile scale and location
	if (bIsRightTurn)
	{
		CurrentTile = World->SpawnActor<ATile>(ATile::StaticClass(), RightSpawnPoint->GetComponentTransform());
		if (CurrentTile)
		{
			CurrentTile->SetActorScale3D(SpawnScale);
			CurrentTile->MoveDirection = FVector{ -1.0f, 0, 0 };
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
			CurrentTile->SetActorScale3D(ScaleConverterBug(SpawnScale));
			CurrentTile->MoveDirection = FVector{ 0, 1.0f, 0 };
			CurrentTile->StartPosition = LeftSpawnPoint->GetComponentTransform().GetLocation();
			CurrentTile->EndPosition = CurrentTile->StartPosition + CurrentTile->MoveDirection * CurrentTile->ReverseDistance;
			CurrentTile->CurrentDestLocation = CurrentTile->EndPosition;
		}
	}
}

void ABlockSpawner::SetCurrentTileLocation()
{
	NewCenter = CurrentTile->GetActorLocation();

	if (bIsRightTurn)
	{
		NewCenter.X = (CurrentTile->GetActorLocation().X + PreviousTile->GetActorLocation().X) / 2;
	}
	else
	{
		NewCenter.Y = (CurrentTile->GetActorLocation().Y + PreviousTile->GetActorLocation().Y) / 2;
	}

	UE_LOG(LogTemp, Warning, TEXT("New Center: %s"), *NewCenter.ToString());	

	//todo Generate intersected tile part and simulate physics
}

void ABlockSpawner::SetCurrentTileScale()
{
	FVector NewScale = PreviousTile->GetActorScale3D();
	float ExtraPartScale;

	if (bIsRightTurn)
	{
		ExtraPartScale = 1 - FMath::Abs(((CurrentTile->GetActorLocation().X + PreviousTile->GetActorLocation().X) / 100));
		NewScale.Y = 1 - ExtraPartScale;
	}
	else
	{
		// Y is X 
		// X is Y 
		// BUG: UE-32676
		ExtraPartScale = FMath::Abs(((CurrentTile->GetActorLocation().Y + PreviousTile->GetActorLocation().Y) / 100));
		NewScale.X = 1 - ExtraPartScale;
	}

	// Fcking hack because of bug
;;	SpawnScale = NewScale;
	SpawnScale.X = NewScale.Y;
	SpawnScale.Y = NewScale.X;

	UE_LOG(LogTemp, Warning, TEXT("Extra part scale: %f"), ExtraPartScale);
	UE_LOG(LogTemp, Warning, TEXT("New Scale: %s"), *SpawnScale.ToString());

	if (bIsRightTurn)
	{
		CurrentTile->SetActorScale3D(SpawnScale);
	}
	else
	{
		CurrentTile->SetActorScale3D(NewScale);
	}
	CurrentTile->SetActorLocation(NewCenter);
}

void ABlockSpawner::UpdateArrowLocations()
{
	if (bIsRightTurn)
	{
		FVector NewLoc = RightSpawnPoint->GetComponentLocation();
		NewLoc.Y = PreviousTile->GetActorLocation().Y;

		RightSpawnPoint->SetWorldLocation(NewLoc);
	}
	else
	{
		FVector NewLoc = LeftSpawnPoint->GetComponentLocation();
		NewLoc.X = PreviousTile->GetActorLocation().X;

		LeftSpawnPoint->SetWorldLocation(NewLoc);
	}
}

bool ABlockSpawner::IsGameOver() const
{
	int32 DefaultExtent = 50; // Our Tile Have Dimensions 100cm x 100cm x 10cm
	float TileExtent;

	UE_LOG(LogTemp, Warning, TEXT("Current Tile Loc: %s"), *CurrentTile->GetActorLocation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Previous Tile Loc: %s"), *PreviousTile->GetActorLocation().ToString());

	float Dist;
	if (bIsRightTurn)
	{
		TileExtent = DefaultExtent * PreviousTile->GetActorScale3D().X;
		Dist = FMath::Abs((CurrentTile->GetActorLocation().X - PreviousTile->GetActorLocation().X)) / 2;
	}
	else
	{
		TileExtent = DefaultExtent * PreviousTile->GetActorScale3D().Y;
		Dist = FMath::Abs((CurrentTile->GetActorLocation().Y - PreviousTile->GetActorLocation().Y)) / 2;
	}	

	UE_LOG(LogTemp, Warning, TEXT("Tile Extent : %f "), TileExtent);

	return Dist >= TileExtent;
}

FVector ABlockSpawner::ScaleConverterBug(FVector & Scale)
{
	FVector NewScale = Scale;
	NewScale.X = Scale.Y;
	NewScale.Y = Scale.X;
	return NewScale;
}

