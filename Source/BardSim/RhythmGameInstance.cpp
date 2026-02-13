// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmGameInstance.h"
#include "Kismet/GameplayStatics.h"

URhythmGameInstance::URhythmGameInstance()
    : playerSaveGame(nullptr)
{
}

void URhythmGameInstance::loadOrCreateSaveGame()
{
	FString slotName = TEXT("playerProgress");
    int32 userIndex = 0;

    if(UGameplayStatics::DoesSaveGameExist(slotName, userIndex))
    {
        playerSaveGame = Cast<URhythmSaveGame>(UGameplayStatics::LoadGameFromSlot(slotName, userIndex));
        if (!playerSaveGame)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame. Creating new one."));
            playerSaveGame = Cast<URhythmSaveGame>(UGameplayStatics::CreateSaveGameObject(URhythmSaveGame::StaticClass()));

            //new game starts with level1 unlocked
            playerSaveGame->unlockedLevels.Add(TEXT("Level1"));

            saveGameProgress();
        }
    }
    else
    {
        playerSaveGame = Cast<URhythmSaveGame>(UGameplayStatics::CreateSaveGameObject(URhythmSaveGame::StaticClass()));
        if(playerSaveGame)
        {
            // Optional: Initialize default unlocked levels
            playerSaveGame->unlockedLevels.Add(TEXT("Level1"));
            saveGameProgress();
        }
	}
}

void URhythmGameInstance::saveGameProgress()
{
    if (playerSaveGame)
    {
        UGameplayStatics::SaveGameToSlot(playerSaveGame, TEXT("playerProgress"), 0);
    }
}

TSet<FName> URhythmGameInstance::getUnlockedLevels()
{
    TSet<FName> emptySet;
    if (playerSaveGame)
    {
        return playerSaveGame->unlockedLevels;
    }
    else
    {
        return emptySet;
    }
}

TSet<FName> URhythmGameInstance::getCollectedSheetMusic()
{
    TSet<FName> emptySet;
    if (playerSaveGame)
    {
        return playerSaveGame->collectedSheetMusic;
    }
    else
    {
        return emptySet;
    }
}

void URhythmGameInstance::unlockLevel(FName& LevelName)
{
    if (playerSaveGame)
    {
        playerSaveGame->unlockedLevels.Add(LevelName);
        saveGameProgress();
    }
}

void URhythmGameInstance::collectSheetMusic(FName& SheetName)
{
    if (playerSaveGame)
    {
        playerSaveGame->collectedSheetMusic.Add(SheetName);
        saveGameProgress();
	}
}

bool URhythmGameInstance::isLevelUnlocked(FName levelName)
{
    return playerSaveGame->unlockedLevels.Contains(levelName);
}

bool URhythmGameInstance::allSheetsCollected()
{
    for (FName& sheetName : levelProgressionOrder)
    {
        if (!(playerSaveGame->collectedSheetMusic.Contains(sheetName)))
        {
            return false;
        }
    }

    return true;
}

void URhythmGameInstance::resetSaveGame()
{
    FString slotName = TEXT("playerProgress");
    int32 userIndex = 0;

    //Delete existing save slot
    bool bDeleted = UGameplayStatics::DeleteGameInSlot(slotName, userIndex);

    UE_LOG(LogTemp, Log, TEXT("SAVE DELETED"));

    //Create new save game object
    playerSaveGame = Cast<URhythmSaveGame>(UGameplayStatics::CreateSaveGameObject(URhythmSaveGame::StaticClass()));
    if (playerSaveGame)
    {
        //Initialise default data
        playerSaveGame->unlockedLevels.Empty();
        playerSaveGame->unlockedLevels.Add(TEXT("Level1"));
        playerSaveGame->collectedSheetMusic.Empty();

        //Save new data
        bool bSaved = UGameplayStatics::SaveGameToSlot(playerSaveGame, slotName, userIndex);
    }
}

bool URhythmGameInstance::doesSaveGameExist() const
{
    FString slotName = TEXT("playerProgress");
    int32 userIndex = 0;

    return UGameplayStatics::DoesSaveGameExist(slotName, userIndex);
}

void URhythmGameInstance::Init()
{
	Super::Init();

    levelProgressionOrder = {FName("Level1"), FName("Level2"), FName("Level3")};

    loadOrCreateSaveGame();
}
