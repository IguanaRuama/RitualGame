// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteActor.h"

const TMap<ENoteDirection, FVector> ANoteActor::spawnLocations =
{
	{ENoteDirection::Left, FVector(-300.f, 0.f, 1000.f)},
	{ENoteDirection::Up, FVector(-100.f, 0.f, 1000.f)},
	{ENoteDirection::Down, FVector(100.f, 0.f, 1000.f)},
	{ENoteDirection::Right, FVector(300.f, 0.f, 1000.f)}
};


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

void ANoteActor::initNote(ENoteDirection inDirection, float inSpeed)
{
	direction = inDirection;
	speed = inSpeed;
	
	//sets spawn location for the note depending on its direction
	SetActorLocation(getSpawnLocation(direction));
}

FVector ANoteActor::getSpawnLocation(ENoteDirection direction)
{
	if (spawnLocations.Contains(direction))
	{
		return spawnLocations[direction];
	}

	return FVector::ZeroVector;
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

	//moves note downwards
	FVector location = GetActorLocation();
	location.Z -= speed * DeltaTime;

	SetActorLocation(location);

}

