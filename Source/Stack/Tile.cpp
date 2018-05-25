// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Curves/CurveLinearColor.h"
#include "TimerManager.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Speed = 200.0f;
	ReverseDistance = 500.0f;
	bCanMove = true;

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> SM_Mesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> ParentMaterial;
		ConstructorHelpers::FObjectFinderOptional<UCurveLinearColor> ColorChangeCurve;
		FConstructorStatics() :
			SM_Mesh(TEXT("StaticMesh'/Game/Assets/Meshes/Tile.Tile'")),
			ParentMaterial(TEXT("Material'/Game/Assets/Materials/M_Tile.M_Tile'")),
			ColorChangeCurve(TEXT("CurveLinearColor'/Game/Assets/Curves/C_TileColorChange.C_TileColorChange'"))
		{
		}
	};

	static FConstructorStatics ConstructorStatics;

	DefaultMaterial = ConstructorStatics.ParentMaterial.Get();

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(RootComponent);
	TileMesh->SetStaticMesh(ConstructorStatics.SM_Mesh.Get());
	TileMesh->SetMobility(EComponentMobility::Movable);
	TileMesh->SetAngularDamping(0.3f);
	TileMesh->SetLinearDamping(0.4f);
	TileMesh->SetMaterial(0, DefaultMaterial);
	TileMesh->SetVisibility(true);
	TileMesh->CastShadow = false;

	// Setting Curve
	if (ConstructorStatics.ColorChangeCurve.Succeeded())
	{
		ColorChangeCurve = ConstructorStatics.ColorChangeCurve.Get();
	}

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{	
	// Setting Timeline Settings
	FOnTimelineLinearColor OnTimelineCallback;
	FOnTimelineEventStatic OnTimelineFinishedCallback;

	Super::BeginPlay();

	if (ColorChangeCurve)
	{
		TileColorChangeTimeline = NewObject<UTimelineComponent>(this, FName("ColorChangeTimeline"));
		TileColorChangeTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		this->BlueprintCreatedComponents.Add(TileColorChangeTimeline);
		TileColorChangeTimeline->SetLooping(true);
		TileColorChangeTimeline->SetTimelineLength(40.0f);
		TileColorChangeTimeline->SetPlaybackPosition(CurrentPlaybackPosition, false);

		OnTimelineCallback.BindUFunction(this, FName("TimelineCallback"));
		OnTimelineFinishedCallback.BindUFunction(this, FName("TimelineFinishedCallback"));

		TileColorChangeTimeline->AddInterpLinearColor(ColorChangeCurve, OnTimelineCallback);
		TileColorChangeTimeline->SetTimelineFinishedFunc(OnTimelineFinishedCallback);
		TileColorChangeTimeline->RegisterComponent();

		PlayTimeline();
	}
}

void ATile::TimelineCallback(FLinearColor Color)
{
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *Color.ToString());
	CurrentPlaybackPosition = TileColorChangeTimeline->GetPlaybackPosition();
	SetColor(Color);
}

void ATile::TimelineFinishedCallback()
{
}

void ATile::PlayTimeline()
{
	if (TileColorChangeTimeline)
	{
		TileColorChangeTimeline->PlayFromStart();
	}
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TileColorChangeTimeline)
	{
		TileColorChangeTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}

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

void ATile::SetColor(FLinearColor Color)
{
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, this);

	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(FName(TEXT("BaseColor")), Color);
		TileMesh->SetMaterial(0, DynamicMaterial);
	}
}

