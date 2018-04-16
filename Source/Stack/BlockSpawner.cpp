// Fill out your copyright notice in the Description page of Project Settings.

#include "BlockSpawner.h"
#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"


// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABlockSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlockSpawner::SetTileCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("Setting tile callback!"));
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

