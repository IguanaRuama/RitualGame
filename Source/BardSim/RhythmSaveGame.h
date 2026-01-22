// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RhythmSaveGame.generated.h"

UCLASS()
class BARDSIM_API URhythmSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<FName> unlockedLevels;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<FName> collectedSheetMusic;
	
};
