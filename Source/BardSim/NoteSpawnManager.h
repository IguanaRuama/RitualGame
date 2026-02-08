// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteTypes.h"
#include "SongDataAsset.h"
#include "NoteSpawnManager.generated.h"

UCLASS(Blueprintable)
class BARDSIM_API ANoteSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteSpawnManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Positions")
	TMap<ENoteDirection, AActor*> spawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Positions")
	TMap<ENoteDirection, AActor*> endLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Positions")
	TMap<ENoteDirection, AActor*> hitLocations;

	//Class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	TSubclassOf<ANoteActor> noteActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	USongDataAsset* currentSongDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float noteSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float leadTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float spawnToHitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float hitToEndTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	float lifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	UDataTable* noteDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	int32 poolSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note Spawning")
	FVector poolLocation;

	// Gets average distance for all directions
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	float getAverageNoteTravelDistance();

	//sets travel times for spawn to hit to end
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void setTravelTimes();

	//Initialises with song asset data and config
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void initialise(USongDataAsset* SongDataAsset, UDataTable* inNoteDataTable, TSubclassOf<ANoteActor> inNoteActorClass, float inSpeed, float inLeadTime);

	//Call every tick from GameMode with current song time to spawn notes
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void processNoteSpawning(float currentSongTime);

	//Spawn pool of notes to take from
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void initialisePool();

	//Get note from pool
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	ANoteActor* getPooledNote();

	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	const TArray<FNoteData>& getNoteDataArray() const;

	//spawn note from pool
	UFUNCTION(BlueprintCallable, Category = "Note Spawning")
	void spawnNote(FNoteData noteData);

	//Current array of note data pointers
	UPROPERTY()
	TArray<FNoteData> noteDataArray;
	

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<ANoteActor*> notePool;

	void loadSongData(UDataTable* inNoteDataTable);

	int32 nextNoteIndex = 0;

	UWorld* cachedWorld = nullptr;

private:
	float getTravelDistanceForDirection(ENoteDirection direction);
};
