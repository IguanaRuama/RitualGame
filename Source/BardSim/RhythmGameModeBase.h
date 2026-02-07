// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "NoteTypes.h"
#include "Sound/SoundBase.h"
#include "NoteInputHandling.h"
#include "NoteSpawnManager.h"
#include "SongDataAsset.h"
#include "RhythmSaveGame.h"
#include "RhythmGameInstance.h"
#include "Components/AudioComponent.h"
#include "CrowdMove.h"
#include "RhythmGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, newCombo, int32, newScore);

USTRUCT(BlueprintType)
struct FSongLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* songDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* songAudio = nullptr;
};


UCLASS(Blueprintable)
class BARDSIM_API ARhythmGameModeBase : public AGameModeBase, public INoteInputHandling, public ICrowdMove
{
	GENERATED_BODY()

public:
	ARhythmGameModeBase();

	//Can be overriden by derived classes
	virtual void BeginPlay() override;
	
	virtual void handleNoteInput_Implementation(ENoteDirection inputDirection, float inputTime) override;
	virtual float getSongTime_Implementation() const override;

	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnScoreChanged onScoreChanged;

	UPROPERTY(BlueprintReadWrite, Category = "Level Progression")
	FName currentLevelName;
	
	//Note to be spawned, limited only to note actor class
	UPROPERTY(EditDefaultsOnly, Category =  "Song")
	TSubclassOf<class ANoteActor> noteActorClass;

	UPROPERTY()
	TArray<UAudioComponent*> instrumentAudioComponents;

	// Instrument audio assets to assign in editor
	UPROPERTY(EditAnywhere, Category = "Song")
	TArray<USoundBase*> instrumentSounds;

	// Accuracy thresholds per instrument
	UPROPERTY(EditAnywhere, Category = "Song")
	TArray<float> instrumentAccuracyThresholds;

	// Track active instruments
	UPROPERTY()
	TArray<bool> instrumentActive;

	UPROPERTY(EditAnywhere, Category = "Song")
	float instrumentFadeDuration;

	//Note speed (units per sec)
	UPROPERTY(BlueprintReadWrite, Category = "Song")
	float noteSpeed;

	//Note speed (units per sec)
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	float bpm;

	//Current score and combo
	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 combo;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 highestCombo;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	float averageAccuracy;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 totalHits;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	bool passed;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	FString grade;

	//Distance note has to travel to hit point
	UPROPERTY(BlueprintReadWrite, Category = "Song")
	float noteTravelDistance = 400.f;

	//Time the note must spawn to have room to lead down screen
	UPROPERTY(BlueprintReadWrite, Category = "Song")
	float leadTime;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 score;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 perfectHits;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 greatHits;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 goodHits;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 misses;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> endScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Crowd")
	TSubclassOf<AActor> crowdActorClasses;

	// Spawned crowd actor instances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	TArray<AActor*> spawnedCrowd;

	// Combo thresholds for crowd presence per part
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	TArray<int32> crowdComboThresholds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	TArray<AActor*> crowdOnscreenPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	TArray<AActor*> crowdOffscreenPoints;

	int32 nextNoteIndex;
	
	float songTime;

	int baseScorePerNote;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	class ANoteSpawnManager* noteSpawnManager;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void updateScoreCombo(int32 newCombo, int32 newScore);

	UFUNCTION(BlueprintCallable, Category = "Level Progression")
	void processLevelUnlock(FName& levelName, bool unlockSheetMusic);

	void handleLevelCompletion(FName& levelName, FString& grade, bool passed);

	UFUNCTION(BlueprintCallable, Category = "Song")
	void startSong(float inInterval);

	// Called when audio finishes
	UFUNCTION(BlueprintNativeEvent, Category = "Song")
	void onSongAudioFinished();
	virtual void onSongAudioFinished_Implementation();

	// Call periodically or after each hit
	UFUNCTION()
	void updateInstrumentLayers();

	UFUNCTION(BlueprintCallable, Category = "Song")
	void initInstrumentAudioComponents();

	// Show the end screen widget; Blueprint implement UI logic
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void showEndScreenWidget();

	// Calculation methods, callable from C++ or Blueprint
	void calculateResults();

	UFUNCTION(BlueprintCallable, Category = "Level Progression")
	FName getNextLevelName();

	UFUNCTION(BlueprintCallable, Category = "Level Progression")
	bool wasSheetMusicUnlockedThisRound();

	//Getter for passed state
	UFUNCTION(BlueprintCallable, Category = "Level Progression")
	bool hasPassed();

	UFUNCTION(BlueprintCallable, Category = "Level Progression")
	bool processEmptyOrExpiredNotes(float currentTime);

	UFUNCTION(BlueprintCallable, Category = "Level Progression")
	bool processNextNoteInput(ENoteDirection inputDirection, float inputTime);

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void updateCrowd(bool comboReset);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pause")
	void togglePause();
	virtual void togglePause_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void pauseTimers();

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void unpauseTimers();

protected:

	//All data relating to current song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	USongDataAsset* currentSongDataAsset;

	//Song DataTable with note timings/directions
	UPROPERTY(BlueprintReadOnly, Category = "Song")
	UDataTable* currentSongDataTable;

	UPROPERTY(BlueprintReadWrite, Category = "Song")
	float timingWindow;

	UPROPERTY(BlueprintReadOnly, Category = "Song")
	float noteSpawningInterval;

	UFUNCTION(BlueprintCallable, Category = "Song")
	void loadSongForLevel(const FName& levelName);

	//Finds spawn manager to reference
	ANoteSpawnManager* findNoteSpawnManager();

	//Camera to be assigned
	ACameraActor* findCamera();

	//timer for spawning notes
	UPROPERTY(BlueprintReadOnly, Category = "Song")
	FTimerHandle noteSpawnTimerHandle;

	//controls timer
	void processNoteSpawningTimer();

	//starts timer
	UFUNCTION(BlueprintCallable, Category = "Song")
	void startNoteSpawningTimer(float interval);

	//stops timer
	UFUNCTION(BlueprintCallable, Category = "Song")
	void stopNoteSpawningTimer();


private:

	bool isPaused = false;

	//Register a successful hit with accuraccy (0-1)
	void registerHit(float accuracy);

	//Register a miss (resets combo)
	void registerMiss();

	bool sheetMusicUnlockedThisLevel;
};
