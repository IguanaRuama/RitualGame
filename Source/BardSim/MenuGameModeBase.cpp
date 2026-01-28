// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameModeBase.h"

AMenuGameModeBase::AMenuGameModeBase()
{
    PrimaryActorTick.bCanEverTick = false;
    DefaultPawnClass = nullptr;
}