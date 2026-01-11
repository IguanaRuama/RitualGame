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

void ANoteSpawnManager::initialisePool()
{
	UWorld* world = GetWorld();

	if (!world || !noteActorClass)
	{
		return;
	}

	notePool.Empty();

	for (int i = 0; i < i < poolSize; ++i)
	{
		ANoteActor* note = world->SpawnActor<ANoteActor>(noteActorClass, FVector::ZeroVector, FRotator::ZeroRotator);

		if (note)
		{
			note->SetActorHiddenInGame(true);
			note->SetActorEnableCollision(false);
			notePool.Add(note);
		}
	}
}

ANoteActor* ANoteSpawnManager::getPooledNote()
{
	for (ANoteActor* note : notePool)
	{
		if (!(note->isActive()))
		{
			return note;
		}
	}

	return nullptr;
}

void ANoteSpawnManager::loadSongData(UDataTable* inNoteDataTable)
{
	noteDataArray.Empty();
	if (inNoteDataTable) //checks if current song has assigned data
	{
		static const FString contextString(TEXT("Song Data")); //ERROR CHECKING

		//retrieves all note data from table and inputs to the array, displays song data if error occurs
		inNoteDataTable->GetAllRows<FNoteData>(contextString, noteDataArray);

		//sorts notes by time in array
		noteDataArray.Sort([](FNoteData* A, FNoteData* B)
			{
				return A->time < B->time;
			});
	}
}

void ANoteSpawnManager::spawnNote(const FNoteData& noteData)
{
	ANoteActor* note = getPooledNote();

	if (note)
	{
		FVector spawnLocation = ANoteActor::getSpawnLocation(noteData.direction);
		note->SetActorLocation(spawnLocation);
		note->SetActorHiddenInGame(false);
		note->initNote(noteData.direction, noteSpeed);
	}
}

void ANoteSpawnManager::removeNote(ANoteActor* note)
{
	if (note)
	{
		note->resetNote();
	}
}

TArray<FNoteData*>& ANoteSpawnManager::getNoteDataArray()
{
	return noteDataArray;
}
