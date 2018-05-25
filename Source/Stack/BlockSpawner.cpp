// Fill out your copyright notice in the Description page of Project Settings.

#include "BlockSpawner.h"
#include "Tile.h"
#include "StackGameModeBase.h"

#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/Rendering/PositionVertexBuffer.h"
#include "TimerManager.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "TransformVectorized.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"



// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ZOffset = 10.0f;
	SpawnScale = FVector{ 1.f };
	AlignmentThreshold = 2;
	bShouldSpawnExtraPart = true;


	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Setting default spawn points (Arrow components)
	RightSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RightSpawnPoint"));
	RightSpawnPoint->SetRelativeLocation(FVector{ 300.0f, 0, 0 });
	RightSpawnPoint->SetRelativeRotation(FRotator{ 0, 180.f, 0 });

	LeftSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftSpawnPoint"));
	LeftSpawnPoint->SetRelativeLocation(FVector{0,-300.0f, 0});
	LeftSpawnPoint->SetRelativeRotation(FRotator{0, 90.f, 0});

	RightSpawnPoint->SetupAttachment(RootComponent);
	LeftSpawnPoint->SetupAttachment(RootComponent);

	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	OurCameraSpringArm->TargetArmLength = 200.f;
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

	SpawnInitialTiles();	
}

void ABlockSpawner::SpawnInitialTiles()
{
	auto World = GetWorld();

	check(World);	

	FTransform FirstTileTransform{ FVector{ 0, 0, -10.0f } };
	PreviousTile = World->SpawnActor<ATile>(ATile::StaticClass(), FirstTileTransform);
	PreviousTile->DisableMovement();

	SpawnTile();
}

void ABlockSpawner::SetTileCallback()
{
	CurrentTile->DisableMovement();

	auto GameMode = Cast<AStackGameModeBase>(GetWorld()->GetAuthGameMode());

	// checking if game is over
	if (IsGameOver())
	{		
		// Setting Simulate Physics for current tile
		CurrentTile->TileMesh->SetSimulatePhysics(true);

		// Move Camera Back
		OurCameraSpringArm->TargetArmLength = 500.0f;

		auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;
		}

		if (GameMode)
		{
			GameMode->IsGameOver = true;
		}


		return;
	}

	// increment tiles count
	if (GameMode)
	{
		GameMode->CurrentTileCount++;
	}


	CheckTileExtraPart();

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

	check(World);	
	
	FTransform SpawnTransform;
	SpawnTransform.SetScale3D(SpawnScale);

	// should spawn based on previuos tile scale and location
	if (bIsRightTurn)
	{
		SpawnTransform.SetTranslation(RightSpawnPoint->GetComponentLocation());

		CurrentTile = World->SpawnActor<ATile>(ATile::StaticClass(), SpawnTransform);
		if (CurrentTile)
		{
			CurrentTile->MoveDirection = FVector{ -1.0f, 0, 0 };
			CurrentTile->StartPosition = RightSpawnPoint->GetComponentTransform().GetLocation();
			CurrentTile->EndPosition = CurrentTile->StartPosition + CurrentTile->MoveDirection * CurrentTile->ReverseDistance;
			CurrentTile->CurrentDestLocation = CurrentTile->EndPosition;
		}
	}
	else
	{
		SpawnTransform.SetTranslation(LeftSpawnPoint->GetComponentLocation());

		CurrentTile = World->SpawnActor<ATile>(ATile::StaticClass(), SpawnTransform);
		if (CurrentTile)
		{										
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
		NewCenter.X = (PreviousTile->GetActorLocation().X + CurrentTile->GetActorLocation().X) / 2;
	}
	else
	{
		NewCenter.Y = (PreviousTile->GetActorLocation().Y + CurrentTile->GetActorLocation().Y) / 2;
	}	
}

void ABlockSpawner::SetCurrentTileScale()
{
	FVector NewScale = PreviousTile->GetActorScale3D();

	if (bIsRightTurn)
	{
		ExtraPartScale = FMath::Abs(((CurrentTile->GetActorLocation().X - PreviousTile->GetActorLocation().X) / 100));
		NewScale.X = NewScale.X - ExtraPartScale;
		ExtraPartScale3D = FVector{ExtraPartScale, NewScale.Y, NewScale.Z};
	}
	else
	{
		ExtraPartScale = FMath::Abs(((CurrentTile->GetActorLocation().Y - PreviousTile->GetActorLocation().Y) / 100));
		NewScale.Y = NewScale.Y - ExtraPartScale;
		ExtraPartScale3D = FVector{ NewScale.X, ExtraPartScale, NewScale.Z};
	}

    SpawnScale = NewScale;

	CurrentTile->SetActorScale3D(SpawnScale);	
	CurrentTile->SetActorLocation(NewCenter);

	if (bShouldSpawnExtraPart)
	{
		GenerateExtraTilePart();
	}

	bShouldSpawnExtraPart = true;
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
	FVector CurrentTileLoc = CurrentTile->GetActorLocation();

	float MinLoc;
	float MaxLoc;

	bool bIsGameOver = false;

	if (bIsRightTurn)
	{
		MinLoc = PreviousTile->GetActorLocation().X - PreviousTile->GetActorScale3D().X * 100;
		MaxLoc = PreviousTile->GetActorLocation().X + PreviousTile->GetActorScale3D().X * 100;
		bIsGameOver = CurrentTileLoc.X >= MaxLoc || CurrentTileLoc.X <= MinLoc;
	}
	else
	{
		MinLoc = PreviousTile->GetActorLocation().Y - PreviousTile->GetActorScale3D().Y * 100;
		MaxLoc = PreviousTile->GetActorLocation().Y + PreviousTile->GetActorScale3D().Y * 100;
		bIsGameOver = CurrentTileLoc.Y >= MaxLoc || CurrentTileLoc.Y <= MinLoc;
	}	

	return bIsGameOver;
}

void ABlockSpawner::GenerateExtraTilePart()
{
	UWorld* World = GetWorld();

	check(World);

	FTransform SpawnTransform;
	FVector Orig = CurrentTile->GetActorLocation();

	float offset;
	float CollisionFixDist = 0.02f;

	if (bIsRightTurn)
	{
		offset = PreviousTile->GetActorScale3D().X * 50 + CollisionFixDist;

		if (PreviousTile->GetActorLocation().X >= CurrentTile->GetActorLocation().X)
		{
			Orig.X -= offset;
		}
		else
		{
			Orig.X += offset;
		}
	}
	else
	{
		offset = PreviousTile->GetActorScale3D().Y * 50 + CollisionFixDist;
		if (PreviousTile->GetActorLocation().Y >= CurrentTile->GetActorLocation().Y)
		{
			Orig.Y -= offset;
		}
		else
		{
			Orig.Y += offset;
		}
	}

	SpawnTransform.SetLocation(Orig);
	SpawnTransform.SetScale3D(ExtraPartScale3D);

	ExtraTile = World->SpawnActor<ATile>(ATile::StaticClass(), SpawnTransform);
	if (ExtraTile)
	{		
		ExtraTile->DisableMovement();		

		FTimerHandle EmptyHandler;
		GetWorldTimerManager().SetTimer(EmptyHandler, this, &ABlockSpawner::EnablePhysics, 0.1f, false);
	}
}

void ABlockSpawner::CheckTileExtraPart()
{
	float Delta;
	if (bIsRightTurn)
	{
		Delta = PreviousTile->GetActorLocation().X - CurrentTile->GetActorLocation().X;
	}
	else
	{
		Delta = PreviousTile->GetActorLocation().Y - CurrentTile->GetActorLocation().Y;
	}

	Delta = FMath::Abs(Delta);

	UE_LOG(LogTemp, Warning, TEXT("Delta: %f"), Delta);

	if (Delta <= AlignmentThreshold)
	{
		FVector PrevTileLoc = PreviousTile->GetActorLocation();
		PrevTileLoc.Z = CurrentTile->GetActorLocation().Z;
		CurrentTile->SetActorLocation(PrevTileLoc);
		bShouldSpawnExtraPart = false;
		UE_LOG(LogTemp, Warning, TEXT("Same Position!"));

		// todo:ashe23 add particle effect on tile set
		// need also keep counter of correct tile set streak and if its count more then 5 , we should enlarge tile scales by 50%
	}
}

void ABlockSpawner::EnablePhysics()
{
	if (ExtraTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Playback Position: %f"), CurrentTile->CurrentPlaybackPosition);
		ExtraTile->CurrentPlaybackPosition = CurrentTile->CurrentPlaybackPosition;
		ExtraTile->TileMesh->SetSimulatePhysics(true);
		//ExtraTile->TileColorChangeTimeline->SetPlaybackPosition(CurrentTile->CurrentPlaybackPosition, false);
	}
}
