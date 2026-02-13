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
	highestCombo = 0;
	averageAccuracy = 0;
	passed = false;
	grade = TEXT("F");
	baseScorePerNote = 100;
	totalHits = 0;
	instrumentAccuracyThresholds = {0.0f, 0.5f, 0.6f, 0.7f};
	crowdComboThresholds = { 10, 30, 60 };
	sheetMusicUnlockedThisLevel = false;
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

	processNextNoteInput(inputDirection, inputTime);
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
	score += baseScorePerNote + (combo * 10) - (FMath::RoundToInt(accuracy * 50));

	updateScoreCombo(combo, score);

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

	if (combo > highestCombo)
	{
		highestCombo = combo;
	}

	updateInstrumentLayers();
	updateCrowd(false);
}

void ARhythmGameModeBase::registerMiss()
{
	combo = 0;
	misses++;
	totalHits++;

	float missAccuracy = 0.0f;

	averageAccuracy = ((averageAccuracy * (totalHits - 1)) + missAccuracy) / totalHits;

	updateScoreCombo(combo, score);
	updateCrowd(true);
	updateInstrumentLayers();
}

void ARhythmGameModeBase::updateScoreCombo(int32 newCombo, int32 newScore)
{
	combo = newCombo;
	score = newScore;
	onScoreChanged.Broadcast(combo, score);
}

void ARhythmGameModeBase::processLevelUnlock(FName& levelName, bool unlockSheetMusic)
{
	URhythmGameInstance* gameInstance = GetGameInstance<URhythmGameInstance>();
	if (!gameInstance || !(gameInstance->playerSaveGame))
	{
		return;
	}

	gameInstance->unlockLevel(levelName);

	TArray<FName>& progressionOrder = gameInstance->levelProgressionOrder;
	int32 currentIndex = progressionOrder.IndexOfByKey(levelName);
	if (progressionOrder.IsValidIndex(currentIndex + 1))
	{
		gameInstance->unlockLevel(progressionOrder[currentIndex + 1]);
	}

	sheetMusicUnlockedThisLevel = unlockSheetMusic;

	if (unlockSheetMusic)
	{
		gameInstance->collectSheetMusic(levelName);
	}

	gameInstance->saveGameProgress();
}


void ARhythmGameModeBase::handleLevelCompletion(FName& levelName, FString& inGrade, bool inPassed)
{
	URhythmGameInstance* gameInstance = GetGameInstance<URhythmGameInstance>();
	if (!inPassed || !(gameInstance->playerSaveGame))
	{
		return;
	}

	bool unlockSheetMusic = (inGrade == TEXT("A") || inGrade == TEXT("S"));
	processLevelUnlock(levelName, unlockSheetMusic);
}

void ARhythmGameModeBase::startSong(float inInterval)
{
	songTime = 0.f;
	perfectHits = 0;
	greatHits = 0;
	goodHits = 0;
	misses = 0;

	startNoteSpawningTimer(inInterval);

	for (int i = 0; i < instrumentAudioComponents.Num(); ++i)
	{
		UAudioComponent* audioComp = instrumentAudioComponents[i];
		if (audioComp)
		{
			float desiredVolume;

			if (instrumentActive.IsValidIndex(i) && instrumentActive[i])
			{
				desiredVolume = 1.0f;
			}
			else
			{
				desiredVolume = 0.0f;
			}

			audioComp->SetVolumeMultiplier(desiredVolume);

			// Play if not already playing to ensure synced playback
			if (!audioComp->IsPlaying())
			{
				audioComp->Play();

				UE_LOG(LogTemp, Log, TEXT("INSTRUMENT PLAYING"));
			}
		}
	}
}

void ARhythmGameModeBase::onSongAudioFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Song playback finished"));
	stopNoteSpawningTimer();

	// Stop all instrument audio components
	for (UAudioComponent* comp : instrumentAudioComponents)
	{
		if (comp && comp->IsPlaying())
		{
			comp->FadeOut(1.f, 0.f);
		}
	}

	calculateResults();

	handleLevelCompletion(currentLevelName, grade, passed);

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController)
	{
		playerController->SetInputMode(FInputModeUIOnly());
		playerController->bShowMouseCursor = true;
		playerController->DisableInput(playerController);
	}

	//UI In Blueprints

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
			instrumentAudioComponents[i]->SetVolumeMultiplier(1.0f);
			instrumentActive[i] = true;

			UE_LOG(LogTemp, Log, TEXT("Instrument %d enabled (Accuracy: %.3f >= %.3f)"), i, averageAccuracy, instrumentAccuracyThresholds[i]);
		}
		else if(!shouldBeActive && instrumentActive[i])
		{
			instrumentAudioComponents[i]->SetVolumeMultiplier(0.0f);
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
	UE_LOG(LogTemp, Log, TEXT("Calculate called"));

	int32 numNotes = 0;
	if (currentSongDataTable)
	{
		numNotes = currentSongDataTable->GetRowNames().Num();
	}

	int32 maxScore = (numNotes * baseScorePerNote) + 10 * (numNotes * (numNotes + 1) / 2);

	// Difficulty factors
	float speedFactor = FMath::Clamp(noteSpeed / 1500.f, 0.f, 0.3f);
	float passThreshold = FMath::Clamp(0.5f + speedFactor, 0.5f, 0.8f);

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

FName ARhythmGameModeBase::getNextLevelName()
{
	URhythmGameInstance* gameInstance = GetGameInstance<URhythmGameInstance>();
	if (!gameInstance)
	{
		return NAME_None;
	}

	TArray<FName>& progressionOrder = gameInstance->levelProgressionOrder;
	int32 currentIndex = progressionOrder.IndexOfByKey(currentLevelName);
	if (progressionOrder.IsValidIndex(currentIndex + 1))
	{
		return progressionOrder[currentIndex + 1];
	}
	return NAME_None;
}

bool ARhythmGameModeBase::wasSheetMusicUnlockedThisRound()
{
	return sheetMusicUnlockedThisLevel;
}

bool ARhythmGameModeBase::hasPassed()
{
	return passed;
}

bool ARhythmGameModeBase::processEmptyOrExpiredNotes(float currentTime)
{
	bool anyMiss = false;
	const TArray<FNoteData>& noteDataArray = noteSpawnManager->getNoteDataArray();

	while (noteDataArray.IsValidIndex(nextNoteIndex))
	{
		const FNoteData& note = noteDataArray[nextNoteIndex];
		if (note.direction == ENoteDirection::None)
		{
			++nextNoteIndex;
			continue;
		}
		if (currentTime > (note.time + timingWindow))
		{
			registerMiss();
			++nextNoteIndex;
			anyMiss = true;
		}
		else
		{
			break;
		}
	}

	return anyMiss;
}

bool ARhythmGameModeBase::processNextNoteInput(ENoteDirection inputDirection, float inputTime)
{
	if (!noteSpawnManager)
	{
		return false;
	}

	const TArray<FNoteData>& noteDataArray = noteSpawnManager->getNoteDataArray();


	if (!noteDataArray.IsValidIndex(nextNoteIndex))
	{
		return false;
	}

	const FNoteData& currentNote = noteDataArray[nextNoteIndex];
	float noteTime = currentNote.time;

	// Ignore input if it's too early (before timing window)
	if (inputTime < noteTime - timingWindow)
	{
		return false;
	}

	// Input within timing window
	if (FMath::Abs(inputTime - noteTime) <= timingWindow)
	{
		if (currentNote.direction == inputDirection)
		{
			float accuracy = FMath::Abs(inputTime - noteTime) / timingWindow;
			registerHit(accuracy);
			++nextNoteIndex;
		}
		else
		{
			registerMiss(); // Wrong direction inside timing window
		}
		return true;
	}

	// Input too late after timing window, ignore
	if (inputTime > noteTime + timingWindow)
	{
		return false;
	}

	return false;
}

void ARhythmGameModeBase::updateCrowd(bool comboReset)
{
	int32 numSections = crowdComboThresholds.Num();

	if (spawnedCrowd.Num() != numSections)
	{
		spawnedCrowd.SetNumZeroed(numSections);
	}

	if (comboReset && combo == 0)
	{
		// Remove only one crowd section
		for (int i = numSections - 1; i >= 0; --i)
		{
			if (spawnedCrowd.IsValidIndex(i) && spawnedCrowd[i] && crowdOnscreenPoints.IsValidIndex(i) && crowdOnscreenPoints[i] && crowdOffscreenPoints.IsValidIndex(i) && crowdOffscreenPoints[i])
			{
				FVector currentLoc = spawnedCrowd[i]->GetActorLocation();
				FVector onscreenLoc = crowdOnscreenPoints[i]->GetActorLocation();

				if (currentLoc.Equals(onscreenLoc))
				{
					ICrowdMove::Execute_startMove(spawnedCrowd[i], crowdOffscreenPoints[i]->GetActorLocation(), crowdOffscreenPoints[i]->GetActorRotation());
					break;  // only move one currently onscreen
				}
			}
		}
		return;
	}

	//Move crowd parts onscreen/offscreen based on combo thresholds
	for (int i = 0; i < numSections; ++i)
	{
		if (!spawnedCrowd.IsValidIndex(i) || !spawnedCrowd[i])
		{
			continue;
		}

		bool shouldBePresent = combo >= crowdComboThresholds[i];

		if (spawnedCrowd[i]->GetClass()->ImplementsInterface(UCrowdMove::StaticClass()) && shouldBePresent)
		{
			ICrowdMove::Execute_startMove(spawnedCrowd[i], crowdOnscreenPoints[i]->GetActorLocation(), crowdOnscreenPoints[i]->GetActorRotation());
		}
		else if (spawnedCrowd[i]->GetClass()->ImplementsInterface(UCrowdMove::StaticClass()) && !shouldBePresent)
		{
			ICrowdMove::Execute_startMove(spawnedCrowd[i], crowdOffscreenPoints[i]->GetActorLocation(), crowdOffscreenPoints[i]->GetActorRotation());
		}
	}
}

void ARhythmGameModeBase::togglePause_Implementation()
{
	bool bIsPaused = UGameplayStatics::IsGamePaused(GetWorld());
	UGameplayStatics::SetGamePaused(GetWorld(), !bIsPaused);
}

void ARhythmGameModeBase::pauseTimers()
{
	GetWorldTimerManager().PauseTimer(noteSpawnTimerHandle);

	// Fade out all instrument audio components
	for (UAudioComponent* audioComp : instrumentAudioComponents)
	{
		if (audioComp && audioComp->IsPlaying())
		{
			audioComp->SetPaused(true); // Pause playback preserving position
		}
	}
}

void ARhythmGameModeBase::unpauseTimers()
{
	// Unpause the note spawning timer
	GetWorldTimerManager().UnPauseTimer(noteSpawnTimerHandle);

	// Fade in all instrument audio components
	for (UAudioComponent* audioComp : instrumentAudioComponents)
	{
		if (audioComp)
		{
			audioComp->SetPaused(false); // Continues playback
		}
	}
}

void ARhythmGameModeBase::loadSongForLevel(const FName& levelName)
{
	currentLevelName = levelName;

	USongDataAsset* songDataAsset = noteSpawnManager->currentSongDataAsset;
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
		float scalingFactor = 0.0015f;
		float minWindow = 0.1f;
		float maxWindow = 0.5f;

		timingWindow = FMath::Clamp(baseWindow + (noteSpeed * scalingFactor), minWindow, maxWindow);

		UE_LOG(LogTemp, Log, TEXT("Timing window initialized to %.3f"), timingWindow);

		UE_LOG(LogTemp, Log, TEXT("Loaded %d instrument sounds for level %s"), instrumentSounds.Num(), *levelName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SongDataAsset is null for level '%s'"), *currentLevelName.ToString());
		currentSongDataAsset = nullptr;
		currentSongDataTable = nullptr;
		instrumentSounds.Empty();
		noteSpeed = 0;
		bpm = 0;
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

		processEmptyOrExpiredNotes(songTime);

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

