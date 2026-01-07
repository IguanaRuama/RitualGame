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
	PrimaryActorTick.bCanEverTick = true;
	combo = 0;
	score = 0;
	nextNoteIndex = 0;
	songTime = 0.f;

}

void ARhythmGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ACameraActor* cameraActor = findCamera();
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController && cameraActor)
	{
		playerController->SetViewTarget(cameraActor);
	}

	noteSpawnManager = findNoteSpawnManager();

	if (noteSpawnManager && currentSongDataTable)
	{
		noteSpawnManager->initialise(currentSongDataTable, noteActorClass, noteSpeed, leadTime);
	}

	if (currentSongAudio)
	{
		UGameplayStatics::PlaySound2D(this, currentSongAudio);
	}

	UE_LOG(LogTemp, Log, TEXT("NoteSpawnManager assigned: %s"), noteSpawnManager ? *noteSpawnManager->GetName() : TEXT("None"));

	UE_LOG(LogTemp, Log, TEXT("WORKING"));

	FString currentLevelNameStr = UGameplayStatics::GetCurrentLevelName(this, true);
	FName currentLevelName(*currentLevelNameStr);

	if (currentSongAudio)
	{
		UGameplayStatics::PlaySound2D(this, currentSongAudio);
	}
}

void ARhythmGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	songTime += DeltaTime;

	if (noteSpawnManager)
	{
		noteSpawnManager->processNoteSpawning(songTime);
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

void ARhythmGameModeBase::startSong()
{
	songTime = 0.F;

	if (noteSpawnManager && noteSpawnManager->noteDataTable)
	{
		noteSpawnManager->initialise(noteSpawnManager->noteDataTable, noteActorClass, noteSpeed, leadTime);
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

	//makes iterator that loops through spawn managers and creates a pointer to the current actor
	//only loops once as only one manager is spawned
	for (TActorIterator<ANoteSpawnManager> spawnIt(GetWorld()); spawnIt; ++spawnIt)
	{
		
		UE_LOG(LogTemp, Log, TEXT("Spawn manager found"));
		return *spawnIt;
		
		break;
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
