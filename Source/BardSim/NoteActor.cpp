// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteActor.h"

const TMap<ENoteDirection, FVector> ANoteActor::spawnLocations =
{
	{ENoteDirection::Left, FVector(-230.f,-200.f, 0.f)}, 
	{ENoteDirection::Up, FVector(-80.f, -200.f, 0.f)},
	{ENoteDirection::Down, FVector(70.f, -200.f, 0.f)},
	{ENoteDirection::Right, FVector(220.f, -200.f, 0.f)}
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

	//moves note downwards on Y axis
	FVector location = GetActorLocation();
	location.Y += speed * DeltaTime;

	SetActorLocation(location);

}

