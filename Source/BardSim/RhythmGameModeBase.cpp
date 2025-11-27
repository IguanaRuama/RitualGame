// Fill out your copyright notice in the Description page of Project Settings.
//Added to git

#include "RhythmGameModeBase.h"
#include "NoteActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "SongDataAsset.h"

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

	noteSpawnManager = findNoteSpawnManager();

	UE_LOG(LogTemp, Log, TEXT("NoteSpawnManager assigned: %s"), noteSpawnManager ? *noteSpawnManager->GetName() : TEXT("None"));

	UE_LOG(LogTemp, Log, TEXT("WORKING"));

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
		noteSpawned(*noteDataArray[nextNoteIndex]);
	}
}

void ARhythmGameModeBase::handleNoteInput_Implementation(ENoteDirection inputDirection, float inputTime)
{
	UE_LOG(LogTemp, Log, TEXT("GameMode received note input: %d at time %f"), (int32)inputDirection, inputTime);

	//200ms timing window
	float timingWindow = 0.2f;

	//If there is no upcoming notes, exit
	if (!noteDataArray.IsValidIndex(nextNoteIndex))
	{
		registerMiss();
		return;
	}

	//Skip notes missed
	while (noteDataArray.IsValidIndex(nextNoteIndex))
	{
		FNoteData* currentNote = noteDataArray[nextNoteIndex];
		if (!currentNote)
		{
			++nextNoteIndex;
			continue;
		}

		float timeDiff = inputTime - currentNote->time;

		//if input is earlier than the window, register miss
		if (timeDiff < (timeDiff - timingWindow))
		{
			registerMiss();
			return;
		}

		//If timeDiff is neg, turns positive and checks if its within timing window 
		if ((FMath::Abs(timeDiff) <= timingWindow))
		{
			//checks direction match
			if (currentNote->direction == inputDirection)
			{
				//0 perfect, 1 imperfect
				float accuracy = (FMath::Abs(timeDiff)) / timingWindow;

				registerHit(accuracy);
				++nextNoteIndex;
				return;
			}

			//Wrong direction but within timing window = missed note, but not incremented
			else
			{
				registerMiss();
				return;
			}
		}

		//If input later than timing window = missed note, then incremented
		if (timeDiff > timingWindow)
		{
			registerMiss();
			++nextNoteIndex;
			continue;
		}
	}

	//No notes macthed / falls out of loop
	registerMiss();

}

float ARhythmGameModeBase::getSongTime_Implementation() const
{
	return songTime;
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

void ARhythmGameModeBase::noteSpawned(const FNoteData& note)
{
	if (noteSpawnManager)
	{
		noteSpawnManager->spawnNote(note, noteActorClass, noteSpeed);
	}
}

void ARhythmGameModeBase::loadSongForLevel(const FName& levelName)
{
	//if the level name is correct, load the song from that level
	if (levelSongMap.Contains(levelName))
	{
		USongDataAsset* songDataAsset = levelSongMap[levelName];
		if (songDataAsset)
		{
			currentSongDataAsset = songDataAsset;
			currentSongDataTable = songDataAsset->noteDataTable;
			currentSongAudio = songDataAsset->songAudio;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SongDataAsset is null for level '%s'"), *levelName.ToString());
			currentSongDataAsset = nullptr;
			currentSongDataTable = nullptr;
			currentSongAudio = nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No song data asset found for level '%s'"), *levelName.ToString());
		currentSongDataAsset = nullptr;
		currentSongDataTable = nullptr;
		currentSongAudio = nullptr;
	}
}

ANoteSpawnManager* ARhythmGameModeBase::findNoteSpawnManager()
{

	UWorld* world = GetWorld();

	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Manager not found."));
		return nullptr;
	}

	for (TActorIterator<ANoteSpawnManager> It(GetWorld()); It; ++It)
	{
		
		UE_LOG(LogTemp, Log, TEXT("Spawn manager found and assigned"));
		return *It;
		
		break;
	}
	return nullptr;
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
