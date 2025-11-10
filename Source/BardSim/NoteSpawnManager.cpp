// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteSpawnManager.h"
#include "NoteActor.h"
#include "Engine/World.h"

// Sets default values
ANoteSpawnManager::ANoteSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ANoteSpawnManager::spawnNote(const FNoteData& note, TSubclassOf<class ANoteActor> noteActorClass, float noteSpeed)
{
	if (!noteActorClass)
		return;

	UWorld* world = GetWorld();
	if (!world)
		return;

	//gets lane/location to spawn based on the note direction
	FVector spawnLocation = ANoteActor::getSpawnLocation(note.direction);

	//controlls spawning behaviour, FLESH OUT LATER
	FActorSpawnParameters params;

	//spawns instance of ANoteActor at the location, no rotation and with given params
	//spawn returns a pointer
	ANoteActor* noteActor = world->SpawnActor<ANoteActor>(noteActorClass, spawnLocation, FRotator::ZeroRotator, params);

	//if previous succeeds, spawned note is initiallised
	if (noteActor)
	{
		noteActor->initNote(note.direction, noteSpeed);
	}
}

