// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteActor.h"

// Sets default values
ANoteActor::ANoteActor()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;

	SetActorEnableCollision(false);

	movementTickInterval = 0.01f;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent =mesh;

}

void ANoteActor::initNote(ENoteDirection inDirection, float inSpeed, float inLeadTime, FVector inPoolLocation, FVector& inSpawnLocation, FVector& inEndLocation, FVector& inHitLocation, float InHitToEndTime)
{

	elapsedTime = 0.f;
	movePhase = ENoteMovePhase::SpawnToHit;

	spawnToHitTime = inLeadTime;
	hitToEndTime = InHitToEndTime;

	direction = inDirection;
	speed = inSpeed;

	poolLocation = inPoolLocation;
	startLocation = inSpawnLocation;
	endLocation = inEndLocation;
	hitLocation = inHitLocation;
	
	//sets spawn location for the note depending on its direction
	SetActorLocation(startLocation);
	SetActorHiddenInGame(false);

	if(direction == ENoteDirection::Left)
	{
		mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
	else if (direction == ENoteDirection::Right)
	{
		mesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	}
	else if (direction == ENoteDirection::Up)
	{
		mesh->SetRelativeRotation(FRotator(0.f, 0, 0.f));
	}
	else if (direction == ENoteDirection::Down)
	{
		mesh->SetRelativeRotation(FRotator(0.f, -180, 0.f));
	}
	else
	{
		mesh->SetRelativeRotation(FRotator::ZeroRotator);
	}

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
	movePhase = ENoteMovePhase::Done;

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
	elapsedTime += movementTickInterval;
	
	if (movePhase == ENoteMovePhase::SpawnToHit)
	{
		float progress = FMath::Clamp(elapsedTime / spawnToHitTime, 0.f, 1.f);
		FVector newLocation = FMath::Lerp(startLocation, hitLocation, progress);
		SetActorLocation(newLocation);

		if (progress >= 1.f)
		{
			movePhase = ENoteMovePhase::HitToEnd;
			elapsedTime = 0.f;
		}
	}
	else if (movePhase == ENoteMovePhase::HitToEnd)
	{
		float progress = FMath::Clamp(elapsedTime / hitToEndTime, 0.f, 1.f);
		FVector newLocation = FMath::Lerp(hitLocation, endLocation, progress);
		SetActorLocation(newLocation);

		if (progress >= 1.f)
		{
			movePhase = ENoteMovePhase::Done;
			resetNote();
		}
	}
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

