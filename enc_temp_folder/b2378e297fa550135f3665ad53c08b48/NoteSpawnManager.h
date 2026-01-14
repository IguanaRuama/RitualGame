// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteTypes.h"
#include "NoteSpawnManager.generated.h"

UCLASS(Blueprintable)
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
	float leadTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float lifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	UDataTable* noteDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	int32 poolSize = 20;

	//Initialises with song asset data and config
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void initialise(UDataTable* inNoteDataTable, TSubclassOf<ANoteActor> inNoteActorClass, float inSpeed, float inLeadTime);

	//Call every tick from GameMode with current song time to spawn notes
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void processNoteSpawning(float currentSongTime);

	//Spawn pool of notes to take from
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void initialisePool();

	//Get note from pool
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	ANoteActor* getPooledNote();

	//return note to pool
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void removeNote(ANoteActor* note);

	//Expose reference for read-only access
	TArray<FNoteData> getNoteDataArray();

	//spawn note from pool
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void spawnNote(FNoteData noteData);
	

protected:

	UPROPERTY()
	TArray<ANoteActor*> notePool;

	void loadSongData(UDataTable* inNoteDataTable);

	//Current array of note data pointers
	UPROPERTY()
	TArray<FNoteData> noteDataArray;

	int32 nextNoteIndex = 0;

	UWorld* cachedWorld = nullptr;
};
