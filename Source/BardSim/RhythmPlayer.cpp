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

void ARhythmPlayer::handleNoteInput_Implementation(ENoteDirection inputDirection, float inputTime)
{
	handleNoteInput(inputDirection);

	UE_LOG(LogTemp, Log, TEXT("Player received note input: %d at time %f"), (int32)inputDirection, inputTime);
}

float ARhythmPlayer::getSongTime_Implementation() const
{
	return 0.0f;
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

void ARhythmPlayer::handleNoteInput(ENoteDirection direction)
{
	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}

	//Getting current gamemode and checking if it has note input interface
	AGameModeBase* gameMode = UGameplayStatics::GetGameMode(world);
	if (gameMode && gameMode->GetClass()->ImplementsInterface(UNoteInputHandling::StaticClass()))
	{
		//Retrieve current song time from GameMode
		float inputTime = INoteInputHandling::Execute_getSongTime(gameMode);

		//Forward direction and InputTime to GameMode
		INoteInputHandling::Execute_handleNoteInput(gameMode, direction, inputTime);

		//ERROR CHECKING
		UE_LOG(LogTemp, Log, TEXT("Forwared input direction %d with time %f to GameMode"), (int32)direction, inputTime);
	}


}


