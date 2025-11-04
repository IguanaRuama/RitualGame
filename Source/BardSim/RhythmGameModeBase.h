// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "NoteTypes.h"
#include "Sound/SoundBase.h"
#include "NoteInputHandling.h"
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
	virtual void Tick(float DeltaTime) override;
	
	void handleNoteInput_Implementation(ENoteDirection inputDirection, float inputTime);
	float getSongTime_Implementation() const;

	//Register a successful hit with accuraccy (0-1)
	void registerHit(float accuracy);

	//Register a miss (resets combo)
	void registerMiss();

	//Map level names (FName) to song data
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	TMap<FName, FSongLevelData> levelSongMap;
	
	//Note to be spawned, limited only to note actor class
	UPROPERTY(EditDefaultsOnly, Category =  "Song")
	TSubclassOf<class ANoteActor> noteActorClass;

	//Note speed (units per sec)
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	float noteSpeed = 400.f;

	//Current score and combo
	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 combo;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 score;

	int32 nextNoteIndex;
	
	float songTime;

protected:

	//Song DataTable with note timings/directions
	UPROPERTY(EditDefaultsOnly, Category = "Song")
	UDataTable* currentSongDataTable;

	UPROPERTY(BlueprintReadOnly, Category = "Song")
	USoundBase* currentSongAudio;

	void loadSongForLevel(const FName& levelName);
	
	//array of note rows from DataTable
	TArray<FNoteData*> noteDataArray;

	//Load note data from DataTable
	void loadSongData();

	//Spawn note actor for noteData
	void spawnNote(const FNoteData& note);

};
