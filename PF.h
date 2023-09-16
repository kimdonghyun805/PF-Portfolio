// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "PFGameMode.h"

const ECollisionChannel BossCollisionChannel = ECC_GameTraceChannel1;
const ECollisionChannel NormalCollisionChannel = ECC_GameTraceChannel2;
const ECollisionChannel CoreCollisionChannel = ECC_GameTraceChannel3;
const ECollisionChannel ItemCollisionChannel = ECC_GameTraceChannel4;

const uint8 NumMaxPlayer = 5;
const uint8 NumMaxNormalCharacter = 4;
const uint8 NumMaxBossCharacter = 1;

const FVector DefaultSpawnLocation = FVector(0.0f, 0.0f, 1000.0f);
/*
UENUM(BlueprintType)
enum class GameResult : uint8
{
	GameNotEnd UMETA(DisplayName = "GameNotEnd"),
	AllCoreDestroyed UMETA(DisplayName = "AllCoreDestroyed"),
	AllNormalFinished UMETA(DisplayName = "AllNormalFinished"),
	BossFinished UMETA(DisplayName = "BossFinished"),
	TimeOver UMETA(DisplayName = "TimeOver")
};
*/