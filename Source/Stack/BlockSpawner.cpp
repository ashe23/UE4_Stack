// Fill out your copyright notice in the Description page of Project Settings.

#include "BlockSpawner.h"
#include "Tile.h"

#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Engine/World.h"


// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ZOffset = 10.0f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Setting default spawn points (Arrow components)
	RightSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RightSpawnPoint"));
	LeftSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftSpawnPoint"));

	RightSpawnPoint->bHiddenInGame = false;
	LeftSpawnPoint->bHiddenInGame = false;

	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	//OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 500.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;
	OurCameraSpringArm->bDoCollisionTest = false;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABlockSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnTile();
	
}

void ABlockSpawner::SetTileCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("Setting tile callback!"));

	// Moving our Pawn Root Component Up By Zoffset
	AddActorWorldOffset(FVector{ 0, 0, ZOffset });

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
	UE_LOG(LogTemp, Warning, TEXT("Spawning Tile"));
	
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
			UE_LOG(LogTemp, Warning, TEXT("Spawned Successfully"));
		}
	}
	else
	{
		CurrentTile = World->SpawnActor<ATile>(ATile::StaticClass(), LeftSpawnPoint->GetComponentTransform());
		if (CurrentTile)
		{
			CurrentTile->MoveDirection = FVector{ -1.0f, 0, 0 };
			UE_LOG(LogTemp, Warning, TEXT("Spawned Successfully"));
		}
	}

	bIsRightTurn = !bIsRightTurn;
}

