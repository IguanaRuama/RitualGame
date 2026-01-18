// Fill out your copyright notice in the Description page of Project Settings.
//Added to git

#include "RhythmGameModeBase.h"
#include "NoteActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "SongDataAsset.h"
#include "Camera/CameraActor.h"

ARhythmGameModeBase::ARhythmGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
	combo = 0;
	score = 0;
	nextNoteIndex = 0;
	songTime = 0.f;
	leadTime = noteTravelDistance / noteSpeed;

}

void ARhythmGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ARhythmGameModeBase::handleNoteInput_Implementation(ENoteDirection inputDirection, float inputTime)
{
	//If no manager, no hit is registered
	if (!noteSpawnManager)
	{
		registerMiss();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("GameMode received note input: %d at time %f"), (int32)inputDirection, inputTime);

	//200ms timing window
	float timingWindow = 0.2f;
	TArray<FNoteData> noteDataArray = noteSpawnManager->getNoteDataArray();

	//If there is no upcoming notes, exit
	if (!noteDataArray.IsValidIndex(nextNoteIndex))
	{
		registerMiss();
		return;
	}

	//Skip notes missed
	while (noteDataArray.IsValidIndex(nextNoteIndex))
	{
		FNoteData currentNote = noteDataArray[nextNoteIndex];
		if (currentNote.direction == ENoteDirection::None)
		{
			++nextNoteIndex;
			continue;
		}

		float timeDiff = inputTime - currentNote.time;

		//if input is earlier than the window, register miss
		if (timeDiff < -timingWindow)
		{
			registerMiss();
			return;
		}

		//If timeDiff is neg, turns positive and checks if its within timing window 
		if ((FMath::Abs(timeDiff) <= timingWindow))
		{
			//checks direction match
			if (currentNote.direction == inputDirection)
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

void ARhythmGameModeBase::startSong(float inInterval)
{
	songTime = 0.f;

	if (currentSongAudio)
	{
		UGameplayStatics::PlaySound2D(this, currentSongAudio);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("startSong: currentSongAudio is null, no music played"));
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
			noteSpeed = songDataAsset->noteSpeed;
			bpm = songDataAsset->bpm;
			leadTime = noteTravelDistance / noteSpeed;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SongDataAsset is null for level '%s'"), *levelName.ToString());
			currentSongDataAsset = nullptr;
			currentSongDataTable = nullptr;
			currentSongAudio = nullptr;
			noteSpeed = 0;
			bpm = 0;
			leadTime = 0;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No song data asset found for level '%s'"), *levelName.ToString());
		currentSongDataAsset = nullptr;
		currentSongDataTable = nullptr;
		currentSongAudio = nullptr;
		noteSpeed = 0;
		bpm = 0;
		leadTime = 0;
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

	//makes iterator that loops through spawn managers and creates a pointer to the current actor
	//only loops once as only one manager is spawned
	for (TActorIterator<ANoteSpawnManager> spawnIt(GetWorld()); spawnIt; ++spawnIt)
	{
		
		UE_LOG(LogTemp, Log, TEXT("Spawn manager found"));
		return *spawnIt;
	}
	return nullptr;
}

//makes iterator that loops through cameras and creates a pointer to the current camera
//only loops once as only one camera is spawned
ACameraActor* ARhythmGameModeBase::findCamera()
{
	for (TActorIterator<ACameraActor> cameraIt(GetWorld()); cameraIt; ++cameraIt)
	{

		UE_LOG(LogTemp, Log, TEXT("Camera found"));
		return *cameraIt;
	}

	return nullptr;
}

void ARhythmGameModeBase::processNoteSpawningTimer()
{
	songTime += GetWorld()->GetDeltaSeconds();

	if (noteSpawnManager)
	{
		float currentSongTime = songTime;
		noteSpawnManager->processNoteSpawning(currentSongTime);
	}
}

void ARhythmGameModeBase::startNoteSpawningTimer(float interval)
{
	UE_LOG(LogTemp, Warning, TEXT("StartingTimeer"));
	GetWorldTimerManager().SetTimer(noteSpawnTimerHandle, this, &ARhythmGameModeBase::processNoteSpawningTimer, interval, true);

}

void ARhythmGameModeBase::stopNoteSpawningTimer()
{
	GetWorldTimerManager().ClearTimer(noteSpawnTimerHandle);
}

