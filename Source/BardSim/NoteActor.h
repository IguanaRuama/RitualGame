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
	FTimerHandle movementTimerHandle;

	//Reference to spawn manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ANoteSpawnManager* spawnManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENoteDirection direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector startLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector endLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Locations")
	FVector poolLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float elapsedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float totalTravelTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float movementTickInterval;

	//init note with direction and speed
	UFUNCTION(BlueprintCallable)
	virtual void initNote(ENoteDirection inDirection, float inSpeed, float inLifeTime, FVector inPoolLocation, FVector& inSpawnLocation, FVector& inEndLocation);

	//resets note state and hides for pooling
	UFUNCTION(BlueprintCallable)
	virtual void resetNote();

	//Returns true if note is in play
	UFUNCTION(BlueprintCallable)
	bool isActive();

	//Sets reference to spawn manager
	UFUNCTION(BlueprintCallable)
	void setSpawnManager(ANoteSpawnManager* manager);

	//updates note travel
	UFUNCTION(BlueprintCallable)
	void updateMovement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	

};
