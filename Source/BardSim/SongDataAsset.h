// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "SongDataAsset.generated.h"

UCLASS()
class BARDSIM_API USongDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//Data table holding note data for specific song
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	UDataTable* noteDataTable;
	
	//Audio asset for specific song
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	USoundBase* songAudio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	FString songName;

	//potential use for animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	float BPM = 0;
};
