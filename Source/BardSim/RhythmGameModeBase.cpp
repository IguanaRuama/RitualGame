// Fill out your copyright notice in the Description page of Project Settings.
//Added to git

#include "RhythmGameModeBase.h"
#include "NoteActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"

ARhythmGameModeBase::ARhythmGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	combo = 0;
	score = 0;
	nextNoteIndex = 0;
	songTime = 0.f;

}

void ARhythmGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FString currentLevelNameStr = UGameplayStatics::GetCurrentLevelName(this, true);
	FName currentLevelName(*currentLevelNameStr);

	//gathers all data for the song in the level
	loadSongForLevel(currentLevelName);
	
	//inputs data to arrays
	loadSongData();

	if (currentSongAudio)
	{
		UGameplayStatics::PlaySound2D(this, currentSongAudio);
	}

}

void ARhythmGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	songTime += DeltaTime;

	//Checks next note exists and that the time matches, increments index to point to next note if yes
	//then spawns note
	for (nextNoteIndex; noteDataArray.IsValidIndex(nextNoteIndex) && noteDataArray[nextNoteIndex]->time <= songTime; ++nextNoteIndex)
	{
		spawnNote(*noteDataArray[nextNoteIndex]);
	}
}

void ARhythmGameModeBase::registerHit(float accuracy)
{
	combo++;

	//score is 100 + (combo multiplier - accuracy multiplier)
	score += 100 + (combo * 10) - (FMath::RoundToInt(accuracy * 50));
}

void ARhythmGameModeBase::registerMiss()
{
	combo = 0;
}

void ARhythmGameModeBase::loadSongForLevel(const FName& levelName)
{
	//if the level name is correct, load the song from that level
	if (levelSongMap.Contains(levelName))
	{
		FSongLevelData& songData = levelSongMap[levelName];
		currentSongDataTable = songData.songDataTable;
		currentSongAudio = songData.songAudio;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No song data found for level '%s'"), *levelName.ToString());
		currentSongDataTable = nullptr;
		currentSongAudio = nullptr;
	}
}

void ARhythmGameModeBase::loadSongData() //TEMP FIX WHEN TESTED AND ADD TABLES
{
	noteDataArray.Empty();
	if (currentSongDataTable) //checks if current song has assigned data
	{
		static const FString contextString(TEXT("Song Data")); //ERROR CHECKING

		//retrieves all note data from table and inputs to the array, displays song data if error occurs
		currentSongDataTable->GetAllRows<FNoteData>(contextString, noteDataArray); 

		//sorts notes by time in array
		noteDataArray.Sort([](const FNoteData& A, const FNoteData& B)
			{
				return A.time < B.time;
			});
	}
}

void ARhythmGameModeBase::spawnNote(const FNoteData& note)
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
