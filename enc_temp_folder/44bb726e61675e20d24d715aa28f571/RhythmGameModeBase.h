// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "NoteTypes.h"
#include "Sound/SoundBase.h"
#include "NoteInputHandling.h"
#include "NoteSpawnManager.h"
#include "SongDataAsset.h"
#include "RhythmGameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FSongLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* songDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* songAudio = nullptr;
};


UCLASS()
class BARDSIM_API ARhythmGameModeBase : public AGameModeBase, public INoteInputHandling
{
	GENERATED_BODY()

public:
	ARhythmGameModeBase();

	//Can be overriden by derived classes
	virtual void BeginPlay() override;
	
	virtual void handleNoteInput_Implementation(ENoteDirection inputDirection, float inputTime) override;
	virtual float getSongTime_Implementation() const override;

	//Map level names (FName) to song data
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	TMap<FName, USongDataAsset*> levelSongMap;
	
	//Note to be spawned, limited only to note actor class
	UPROPERTY(EditDefaultsOnly, Category =  "Song")
	TSubclassOf<class ANoteActor> noteActorClass;

	//Note speed (units per sec)
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	float noteSpeed = 400.f;

	//Current score and combo
	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 combo;

	//Distance note has to travel to hit point
	UPROPERTY(BlueprintReadOnly, Category = "Song")
	float noteTravelDistance = 400.f;

	//Time the note must spawn to have room to lead down screen
	UPROPERTY(BlueprintReadOnly, Category = "Song")
	float leadTime;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 score;

	int32 nextNoteIndex;
	
	float songTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	class ANoteSpawnManager* noteSpawnManager;

	UFUNCTION(BlueprintCallable, Category = "Song")
	void startSong();

protected:

	//All data relating to current song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song")
	USongDataAsset* currentSongDataAsset;

	//Song DataTable with note timings/directions
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	UDataTable* currentSongDataTable;

	UPROPERTY(BlueprintReadOnly, Category = "Song")
	USoundBase* currentSongAudio;

	void loadSongForLevel(const FName& levelName);

	//Finds spawn manager to reference
	ANoteSpawnManager* findNoteSpawnManager();

	//Camera to be assigned
	ACameraActor* findCamera();

	//timer for spawning notes
	FTimerHandle noteSpawnTimerHandle;

	//controls timer
	void processNoteSpawningTimer();

	//starts timer
	void startNoteSpawningTimer(float interval);

	void stopNoteSpawningTimer();

private:

	//Register a successful hit with accuraccy (0-1)
	void registerHit(float accuracy);

	//Register a miss (resets combo)
	void registerMiss();

};
