// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CrowdMove.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UCrowdMove : public UInterface
{
	GENERATED_BODY()
};

class BARDSIM_API ICrowdMove
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Crowd")
	void startMove(FVector targetLocation, FRotator targetRotation);
};
