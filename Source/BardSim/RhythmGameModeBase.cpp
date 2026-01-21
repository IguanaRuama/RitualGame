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
	highestCombo = 0;
	averageAccuracy = 0;
	passed = false;
	grade = TEXT("F");
	baseScorePerNote = 100;
	totalHits = 0;
	instrumentFadeDuration = 1.f;
	instrumentAccuracyThresholds = {0.0f, 0.5f, 0.6f, 0.7f};
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

	UE_LOG(LogTemp, Log, TEXT("Timing Window: %f"), timingWindow);

	TArray<FNoteData> noteDataArray = noteSpawnManager->getNoteDataArray();

	//If there is no upcoming notes, exit
	if (!noteDataArray.IsValidIndex(nextNoteIndex))
	{
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
}

float ARhythmGameModeBase::getSongTime_Implementation() const
{
	return songTime;
}

void ARhythmGameModeBase::registerHit(float accuracy)
{
	combo++;
	totalHits++;
	averageAccuracy = ((averageAccuracy * (totalHits - 1)) + (1.0f - accuracy)) / totalHits;

	if (accuracy < 0.2f)
	{
		perfectHits++;
	}
	else if (accuracy < 0.5f)
	{
		greatHits++;
	}
	else if (accuracy < 0.8f)
	{
		goodHits++;
	}
	else
	{
		registerMiss();
	}

	//score is 100 + (combo multiplier - accuracy multiplier)
	score += baseScorePerNote + (combo * 10) - (FMath::RoundToInt(accuracy * 50));

	if (combo > highestCombo)
	{
		highestCombo = combo;
	}

	updateInstrumentLayers();
}

void ARhythmGameModeBase::registerMiss()
{
	combo = 0;
	misses++;
}

void ARhythmGameModeBase::startSong(float inInterval)
{
	songTime = 0.f;
	perfectHits = 0;
	greatHits = 0;
	goodHits = 0;
	misses = 0;

	if (instrumentAudioComponents.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("startSong: no instruments found"));
	}

	startNoteSpawningTimer(inInterval);

}

void ARhythmGameModeBase::onSongAudioFinished()
{
	UE_LOG(LogTemp, Log, TEXT("Song playback finished"));
	stopNoteSpawningTimer();

	// Stop all instrument audio components
	for (UAudioComponent* comp :instrumentAudioComponents)
	{
		if (comp && comp->IsPlaying())
		{
			comp->FadeOut(1.f, 0.f);
		}
	}

	calculateResults();

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController)
	{
		playerController->SetInputMode(FInputModeUIOnly());
		playerController->bShowMouseCursor = true;
		playerController->DisableInput(playerController);
	}

	showEndScreenWidget();

}

void ARhythmGameModeBase::updateInstrumentLayers()
{
	for (int i = 0; i < instrumentAudioComponents.Num(); i++)
	{
		if (!instrumentAudioComponents[i])
		{
			continue;
		}

		bool shouldBeActive = instrumentAccuracyThresholds.IsValidIndex(i) && averageAccuracy >= instrumentAccuracyThresholds[i];

		if (shouldBeActive && !instrumentActive[i])
		{
			instrumentAudioComponents[i]->FadeIn(instrumentFadeDuration, 1.0f);
			instrumentActive[i] = true;

			UE_LOG(LogTemp, Log, TEXT("Instrument %d enabled (Accuracy: %.3f >= %.3f)"), i, averageAccuracy, instrumentAccuracyThresholds[i]);
		}
		else if(!shouldBeActive && instrumentActive[i])
		{
			instrumentAudioComponents[i]->FadeOut(instrumentFadeDuration, 0.0f);
			instrumentActive[i] = false;

			UE_LOG(LogTemp, Log, TEXT("Instrument %d disabled (Accuracy: %.3f < %.3f)"), i, averageAccuracy, instrumentAccuracyThresholds[i]);
		}
	}
}

void ARhythmGameModeBase::initInstrumentAudioComponents()
{
	//Stop sound playing and remoce from actor
	for (UAudioComponent* comp : instrumentAudioComponents)
	{
		if (comp)
		{
			comp->Stop();
			comp->DestroyComponent();
		}
	}

	//Clears array
	instrumentAudioComponents.Empty();
	instrumentActive.Empty();

	// Create new AudioComponents for each InstrumentSound
	for (int i = 0; i < instrumentSounds.Num(); ++i)
	{
		USoundBase* sound = instrumentSounds[i];
		if (sound)
		{
			UAudioComponent* audioComp = NewObject<UAudioComponent>(this);
			audioComp->bAutoActivate = false;
			audioComp->bAutoDestroy = false;
			audioComp->RegisterComponent();
			audioComp->SetSound(sound);

			if (i == 0)
			{
				//Start full volume
				audioComp->SetVolumeMultiplier(1.f);
				audioComp->Play();
				instrumentActive.Add(true);
				audioComp->OnAudioFinished.AddDynamic(this, &ARhythmGameModeBase::onSongAudioFinished);
			}
			else
			{
				//Start muted for sync
				audioComp->SetVolumeMultiplier(0.f);
				audioComp->Play();
				instrumentActive.Add(false);
			}

			instrumentAudioComponents.Add(audioComp);

		}
		else
		{
			instrumentAudioComponents.Add(nullptr);
			instrumentActive.Add(false);
		}
	}
}

void ARhythmGameModeBase::calculateResults()
{
	int32 numNotes = 0;
	if (currentSongDataTable)
	{
		numNotes = currentSongDataTable->GetRowNames().Num();
	}

	int32 maxScore = (numNotes * baseScorePerNote) + (10 * (numNotes * (numNotes + 1) / 2));

	// Difficulty factors
	float speedFactor = FMath::Clamp(noteSpeed / 500.f, 0.f, 0.2f);
	float passThreshold = FMath::Clamp(0.6f + speedFactor, 0.6f, 0.8f);

	// Combine multipliers for passing score
	int32 passingScore = FMath::RoundToInt(maxScore * passThreshold);

	UE_LOG(LogTemp, Log, TEXT("Score needed: %i"), passingScore);

	UE_LOG(LogTemp, Log, TEXT("Avg accuracy: %f"), averageAccuracy);

	passed = (score >= passingScore);

	if (averageAccuracy >= 0.95f)
	{
		grade = TEXT("S");
	}
	else if (averageAccuracy >= 0.90f)
	{
		grade = TEXT("A");
	}
	else if (averageAccuracy >= 0.80f)
	{
		grade = TEXT("B");
	}
	else if (averageAccuracy >= 0.70f)
	{
		grade = TEXT("C");
	}
	else if (averageAccuracy >= 0.60f)
	{
		grade = TEXT("D");
	}
	else
	{
		grade = TEXT("F");
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
			instrumentSounds = songDataAsset->instrumentSounds;
			currentSongDataTable = songDataAsset->noteDataTable;
			noteSpeed = songDataAsset->noteSpeed;
			bpm = songDataAsset->bpm;

			instrumentAudioComponents.Empty();
			instrumentActive.Empty();

			float baseWindow = 0.1f;
			float scalingFactor = 0.0005f;
			float minWindow = 0.1f;
			float maxWindow = 1.f;

			timingWindow = FMath::Clamp(baseWindow + (noteSpeed * scalingFactor), minWindow, maxWindow);

			UE_LOG(LogTemp, Log, TEXT("Timing window initialized to %.3f"), timingWindow);

			UE_LOG(LogTemp, Log, TEXT("Loaded %d instrument sounds for level %s"), instrumentSounds.Num(), *levelName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SongDataAsset is null for level '%s'"), *levelName.ToString());
			currentSongDataAsset = nullptr;
			currentSongDataTable = nullptr;
			instrumentSounds.Empty();
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
		instrumentSounds.Empty();
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
	songTime += noteSpawningInterval;
	

	if (noteSpawnManager)
	{
		float currentSongTime = songTime;
		noteSpawnManager->processNoteSpawning(currentSongTime);
	}
}

void ARhythmGameModeBase::startNoteSpawningTimer(float interval)
{
	noteSpawningInterval = interval;
	GetWorldTimerManager().SetTimer(noteSpawnTimerHandle, this, &ARhythmGameModeBase::processNoteSpawningTimer, interval, true);

}

void ARhythmGameModeBase::stopNoteSpawningTimer()
{
	GetWorldTimerManager().ClearTimer(noteSpawnTimerHandle);
}

