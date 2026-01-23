// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RhythmSaveGame.h"
#include "RhythmGameInstance.generated.h"

UCLASS()
class BARDSIM_API URhythmGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URhythmGameInstance();

	UPROPERTY(BlueprintReadOnly)
	URhythmSaveGame* playerSaveGame;

	UFUNCTION(BlueprintCallable)
	void loadOrCreateSaveGame();

	UFUNCTION(BlueprintCallable)
	void saveGameProgress();

	UFUNCTION(BlueprintCallable)
	TSet<FName> getUnlockedLevels();

	UFUNCTION(BlueprintCallable)
	TSet<FName> getCollectedSheetMusic();

	UFUNCTION(BlueprintCallable)
	void unlockLevel(FName& LevelName);

	UFUNCTION(BlueprintCallable)
	void collectSheetMusic(FName& SheetName);

	//Override Init to load on game start
	virtual void Init() override;
	
};
