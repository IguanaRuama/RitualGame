// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "SongDataAsset.generated.h"

UCLASS(Blueprintable)
class BARDSIM_API USongDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	//Instrument sounds assigned per song asset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	TArray<USoundBase*> instrumentSounds;

	//Data table holding note data for specific song
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	UDataTable* noteDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	FString songName;

	//potential use for animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	float bpm = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Song Data")
	float noteSpeed = 0;
};
