// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "NoteTypes.h"
#include "RhythmPlayer.generated.h"

UCLASS()
class BARDSIM_API ARhythmPlayer : public ACharacter, public INoteInputHandling
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARhythmPlayer();


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:	

	void onLeftPressed();
	void onUpPressed();
	void onDownPressed();
	void onRightPressed();

	void handleNoteInput(ENoteDirection direction);

};
