// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteTypes.h"
#include "NoteSpawnManager.h"
#include "NoteActor.generated.h"

UCLASS(Blueprintable)
class BARDSIM_API ANoteActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Timer handle for note life duration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTimerHandle lifeTimerHandle;

	//Reference to spawn manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ANoteSpawnManager* spawnManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENoteDirection direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector spawnLocationLeft = FVector(-230.f, -200.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector spawnLocationUp = FVector(-80.f, -200.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector spawnLocationDown = FVector(70.f, -200.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector spawnLocationRight = FVector(220.f, -200.f, 0.f);

	//Get fixed spawn location for each lane
	FVector getSpawnLocation(ENoteDirection inDirection);

	//init note with direction and speed
	UFUNCTION(BlueprintCallable)
	virtual void initNote(ENoteDirection inDirection, float inSpeed, float inLifeTime);

	//resets note state and hides for pooling
	UFUNCTION(BlueprintCallable)
	virtual void resetNote();

	//Returns true if note is in play
	UFUNCTION(BlueprintCallable)
	bool isActive();

	//Sets reference to spawn manager
	UFUNCTION(BlueprintCallable)
	void setSpawnManager(ANoteSpawnManager* manager);

	//Called when life timer expires
	UFUNCTION(BlueprintCallable)
	void onLifeTimerExpired();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Sets spawn locations (value) to be found by directions (Key)
	//Assigned in blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	TMap<ENoteDirection, FVector> spawnLocations;

private:
	

};
