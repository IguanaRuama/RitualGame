// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteTypes.h"
#include "NoteSpawnManager.generated.h"

UCLASS()
class BARDSIM_API ANoteSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteSpawnManager();



	//Class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	TSubclassOf<ANoteActor> noteActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float noteSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float leadTime = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	UDataTable* noteDataTable;

	//Initialises with song asset data and config
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void initialise(UDataTable* inNoteDataTable, TSubclassOf<ANoteActor> inNoteActorClass, float inSpeed, float inLeadTime);

	//Call every tick from GameMode with current song time to spawn notes
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void processNoteSpawning(float currentSongTime);

	//Load and sort note data
	void loadSongData(UDataTable* inNoteDataTable);

protected:

	//Spawn one note actor from note data
	UFUNCTION(BlueprintCallable)
	void spawnNote(const FNoteData& note);

	//Current array of note data pointers
	TArray<FNoteData*> noteDataArray;

	int32 nextNoteIndex = 0;

	UWorld* cachedWorld = nullptr;
};
