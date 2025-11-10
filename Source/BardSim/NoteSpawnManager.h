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

	UFUNCTION(BlueprintCallable)
	void spawnNote(const FNoteData& note, TSubclassOf<class ANoteActor> noteActorClass, float noteSpeed);
};
