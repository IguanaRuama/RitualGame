// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARhythmPlayer::ARhythmPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called to bind functionality to input
void ARhythmPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

