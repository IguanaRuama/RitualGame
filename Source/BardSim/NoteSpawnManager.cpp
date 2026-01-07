// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteSpawnManager.h"
#include "NoteActor.h"
#include "Engine/World.h"

// Sets default values
ANoteSpawnManager::ANoteSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANoteSpawnManager::initialise(UDataTable* inNoteDataTable, TSubclassOf<ANoteActor> inNoteActorClass, float inSpeed, float inLeadTime)
{
	noteActorClass = inNoteActorClass;
	noteSpeed = inSpeed;
	leadTime = inLeadTime;
	nextNoteIndex = 0;

	loadSongData(inNoteDataTable);
}

void ANoteSpawnManager::processNoteSpawning(float currentSongTime)
{
	if (noteDataArray.Num() == 0 || !noteActorClass)
	{
		return;
	}

	//Binary search boundaries
	int32 low = nextNoteIndex;
	int32 high = noteDataArray.Num() - 1;
	int32 spawnUpToIndex = nextNoteIndex - 1;

	while (low <= high)
	{
		int32 mid = (low + high) / 2;
		float spawnTime = noteDataArray[mid]->time - leadTime;

		if (spawnTime <= currentSongTime)
		{
			spawnUpToIndex = mid;
			low = mid + 1;
		}
		else
		{
			high = mid - 1;
		}
	}

	//Spawn notes sequentially from nextNoteIndex up to spawnUpToIndex

	for (; nextNoteIndex <= spawnUpToIndex; ++nextNoteIndex)
	{
		spawnNote(*noteDataArray[nextNoteIndex]);
	}
}

void ANoteSpawnManager::loadSongData(UDataTable* inNoteDataTable)
{
}

void ANoteSpawnManager::spawnNote(const FNoteData& note)
{
	if (!cachedWorld || !noteActorClass)
	{
		return;
	}

	FVector spawnLocation = ANoteActor::getSpawnLocation(note.direction);
	ANoteActor* spawnedNote = cachedWorld->SpawnActor<ANoteActor>(noteActorClass, spawnLocation, FRotator::ZeroRotator);

	if (spawnedNote)
	{
		spawnedNote->initNote(note.direction, noteSpeed);
	}
}
