// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteSpawnManager.h"
#include "NoteActor.h"
#include "Engine/World.h"

// Sets default values
ANoteSpawnManager::ANoteSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	poolSize = 20;
}

float ANoteSpawnManager::getAverageNoteTravelDistance()
{
	float totalDistance = 0.f;
	int32 count = 0;

	for (auto& Elem : spawnLocations)
	{
		float distance = getTravelDistanceForDirection(Elem.Key);
		if (distance > 0.f)
		{
			totalDistance += distance;
			++count;
		}
	}

	if (count > 0)
	{
		return (totalDistance / count);
	}
	else
	{
		return 0.f;
	}
}

void ANoteSpawnManager::setTravelTimes()
{
	if (noteSpeed <= 0.f)
	{
		return;
	}

	ENoteDirection exampleDir = ENoteDirection::Left;

	if ((spawnLocations.Contains(exampleDir)) && (hitLocations.Contains(exampleDir)) && (endLocations.Contains(exampleDir)))
	{
		FVector spawnLocation = spawnLocations[exampleDir]->GetActorLocation();
		FVector endLocation = endLocations[exampleDir]->GetActorLocation();
		FVector hitLocation = hitLocations[exampleDir]->GetActorLocation();
	}
}

void ANoteSpawnManager::initialise(USongDataAsset* songDataAsset, UDataTable* inNoteDataTable, TSubclassOf<ANoteActor> inNoteActorClass, float inSpeed, float inLeadTime)
{
	currentSongDataAsset = songDataAsset;
	noteActorClass = inNoteActorClass;
	noteSpeed = inSpeed;
	leadTime = inLeadTime;
	nextNoteIndex = 0;
	lifeTime = leadTime;
	noteDataTable = inNoteDataTable;


	loadSongData(noteDataTable);
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
		float spawnTime = noteDataArray[mid].time - leadTime;

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
		spawnNote(noteDataArray[nextNoteIndex]);

		UE_LOG(LogTemp, Warning, TEXT("spawning note: %f"), currentSongTime);
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

	for (int i = 0; i < poolSize; ++i)
	{
		ANoteActor* note = world->SpawnActor<ANoteActor>(noteActorClass, poolLocation, FRotator::ZeroRotator);

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

void ANoteSpawnManager::BeginPlay()
{
	Super::BeginPlay();
}

void ANoteSpawnManager::loadSongData(UDataTable* inNoteDataTable)
{
	if(!inNoteDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("loadSongData invalid Data Table pointer"));
		return;
	}

	TArray<FNoteData*> tempNotePointers;

	//retrieves all note data from table and inputs to the array, displays song data if error occurs
	static const FString contextString(TEXT("Song Data")); //ERROR CHECKING
	inNoteDataTable->GetAllRows<FNoteData>(contextString, tempNotePointers);

	noteDataArray.Empty();

	for (FNoteData* notePointer : tempNotePointers)
	{
		if (notePointer)
		{
			noteDataArray.Add(*notePointer);
		}
	}

	//sorts notes by time in array
	noteDataArray.Sort([](FNoteData A, FNoteData B)
	{
		return A.time < B.time;
	});

}

float ANoteSpawnManager::getTravelDistanceForDirection(ENoteDirection direction)
{
	AActor** spawnActorPtr = spawnLocations.Find(direction);
	AActor** endActorPtr = endLocations.Find(direction);

	if (spawnActorPtr && endActorPtr && *spawnActorPtr && *endActorPtr)
	{
		return FVector::Dist((*spawnActorPtr)->GetActorLocation(), (*endActorPtr)->GetActorLocation());
	}
	return 0.f;

}

void ANoteSpawnManager::spawnNote(FNoteData noteData)
{
	ANoteActor* note = getPooledNote();

	if (note)
	{
		FVector spawnLocation = FVector::ZeroVector;
		FVector endLocation = FVector::ZeroVector;
		FVector hitLocation = FVector::ZeroVector;

		if (AActor** foundSpawn = spawnLocations.Find(noteData.direction))
		{
			if (*foundSpawn)
			{
				spawnLocation = (*foundSpawn)->GetActorLocation();
			}
			
		}
		if (AActor** foundEnd = endLocations.Find(noteData.direction))
		{
			if (*foundEnd)
			{
				endLocation = (*foundEnd)->GetActorLocation();
			}
		}
		if (AActor** foundHit = hitLocations.Find(noteData.direction))
		{
			if (*foundHit)
			{
				hitLocation = (*foundHit)->GetActorLocation();
			}
		}

		spawnToHitTime = 0.f;
		hitToEndTime = 0.f;

		if (noteSpeed > 0.f)
		{
			spawnToHitTime = FVector::Dist(spawnLocation, hitLocation) / noteSpeed;
			hitToEndTime = FVector::Dist(hitLocation, endLocation) / noteSpeed;
		}

		note->SetActorLocation(spawnLocation);
		note->SetActorHiddenInGame(false);

		note->setSpawnManager(this);

		note->initNote(noteData.direction, noteSpeed, lifeTime, poolLocation, spawnLocation, endLocation, hitLocation, hitToEndTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No available pooled notes! Pool size: %d"), notePool.Num());
	}
}

TArray<FNoteData> ANoteSpawnManager::getNoteDataArray()
{
	return noteDataArray;
}


