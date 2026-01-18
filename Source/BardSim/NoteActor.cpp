// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteActor.h"

// Sets default values
ANoteActor::ANoteActor()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;

	SetActorEnableCollision(false);

	//Make mesh and sets as a parent for others
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	movementTickInterval = 0.01f;

}

void ANoteActor::initNote(ENoteDirection inDirection, float inSpeed, float inLifeTime, FVector inPoolLocation, FVector& inSpawnLocation, FVector& inEndLocation)
{

	UE_LOG(LogTemp, Log, TEXT("Started movement timer for note %p"), this);

	elapsedTime = 0.f;
	totalTravelTime = inLifeTime;
	direction = inDirection;
	speed = inSpeed;
	poolLocation = inPoolLocation;

	startLocation = inSpawnLocation;
	endLocation = inEndLocation;
	
	//sets spawn location for the note depending on its direction
	SetActorLocation(startLocation);
	SetActorHiddenInGame(false);

	if(GetWorld())
	{
		//Sets timer to remove note after certain duration
		GetWorld()->GetTimerManager().SetTimer(movementTimerHandle, this, &ANoteActor::updateMovement, movementTickInterval, true);
	}
	else if (!GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld() invalid when setting timer!"));
		return;
	}
}

void ANoteActor::resetNote()
{
	if(GetWorld())
	{
		//Clears life timer
		GetWorld()->GetTimerManager().ClearTimer(movementTimerHandle);
	}

	SetActorHiddenInGame(true);
	SetActorLocation(poolLocation);
	direction = ENoteDirection::None;

}

bool ANoteActor::isActive()
{
	return !IsHidden();
}

void ANoteActor::setSpawnManager(ANoteSpawnManager* manager)
{
	spawnManager = manager;
}

void ANoteActor::updateMovement()
{
	UE_LOG(LogTemp, Log, TEXT("updateMovement called for note %p"), this);

	elapsedTime += movementTickInterval;
	
	float progress = FMath::Clamp(elapsedTime / totalTravelTime, 0.f, 1.f);

	FVector newLocation = FMath::Lerp(startLocation, endLocation, progress);

	SetActorLocation(newLocation);

	if (progress >= 1.f)
	{
		resetNote();
	}

	UE_LOG(LogTemp, Log, TEXT("Note %p moving to location: %s, progress: %f"), this, *newLocation.ToString(), progress);
}

// Called when the game starts or when spawned
void ANoteActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANoteActor::Tick(float DeltaTime) //UPDATE WITH STUFF FROM YOUTUBE VID FOR DIFF FPS
{
	Super::Tick(DeltaTime);

}

