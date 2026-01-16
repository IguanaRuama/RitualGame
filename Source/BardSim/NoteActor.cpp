// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteActor.h"

// Sets default values
ANoteActor::ANoteActor()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(false);

	//Make mesh and sets as a parent for others
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

}

void ANoteActor::initNote(ENoteDirection inDirection, float inSpeed, float inLifeTime)
{
	direction = inDirection;
	speed = inSpeed;
	
	//sets spawn location for the note depending on its direction
	SetActorLocation(getSpawnLocation(direction));

	SetActorHiddenInGame(false);

	if(GetWorld())
	{
		//Sets timer to remove note after certain duration
		GetWorld()->GetTimerManager().SetTimer(lifeTimerHandle, this, &ANoteActor::onLifeTimerExpired, inLifeTime, false);
	}
}

void ANoteActor::resetNote()
{
	if(GetWorld())
	{
		//Clears life timer
		GetWorld()->GetTimerManager().ClearTimer(lifeTimerHandle);
	}

	SetActorHiddenInGame(true);

}

FVector ANoteActor::getSpawnLocation(ENoteDirection inDirection)
{
	if (spawnLocations.Contains(direction))
	{
		return spawnLocations[direction];
	}

	return FVector::ZeroVector;
}

bool ANoteActor::isActive()
{
	return !IsHidden();
}

void ANoteActor::setSpawnManager(ANoteSpawnManager* manager)
{
	spawnManager = manager;
}

void ANoteActor::onLifeTimerExpired()
{
	if(spawnManager)
	{
		spawnManager->removeNote(this);
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

