// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NoteTypes.generated.h"

UENUM(BlueprintType)
enum class ENoteDirection : uint8
{
	Left,
	Up,
	Down,
	Right,
	None
};

USTRUCT(BlueprintType)
struct FNoteData : public FTableRowBase
{
	GENERATED_BODY()

	//The direction of the note
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENoteDirection direction;

	//Set time (in seconds) the note comes in at
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float time;
};

