// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteTypes.h"
#include "NoteActor.generated.h"

UCLASS()
class BARDSIM_API ANoteActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENoteDirection direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float speed;

	//Get fixed spawn location for each lane
	static FVector getSpawnLocation(ENoteDirection direction);

	//init note with direction and speed
	UFUNCTION(BlueprintCallable)
	virtual void initNote(ENoteDirection inDirection, float inSpeed);

	//resets note state and hides for pooling
	UFUNCTION(BlueprintCallable)
	virtual void resetNote();

	//Returns true if note is in play
	UFUNCTION(BlueprintCallable)
	bool isActive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Sets spawn locations (value) to be found by directions (Key), remains const, cannot be edited
	static const TMap<ENoteDirection, FVector> spawnLocations;

};
