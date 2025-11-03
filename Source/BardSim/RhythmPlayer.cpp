// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmPlayer.h"
#include "RhythmGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARhythmPlayer::ARhythmPlayer()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called to bind functionality to input
void ARhythmPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &ARhythmPlayer::onLeftPressed);
	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &ARhythmPlayer::onUpPressed);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &ARhythmPlayer::onDownPressed);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &ARhythmPlayer::onRightPressed);
}

void ARhythmPlayer::onLeftPressed()
{
	handleNoteInput(ENoteDirection::Left);
}

void ARhythmPlayer::onUpPressed()
{
	handleNoteInput(ENoteDirection::Up);
}

void ARhythmPlayer::onDownPressed()
{
	handleNoteInput(ENoteDirection::Down);
}

void ARhythmPlayer::onRightPressed()
{
	handleNoteInput(ENoteDirection::Right);
}


