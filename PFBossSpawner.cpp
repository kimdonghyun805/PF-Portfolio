// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBossSpawner.h"
#include "Components/CapsuleComponent.h"

APFBossSpawner::APFBossSpawner()
{
	// 보스 캐릭터의 스폰 위치 지정
	GetCapsuleComponent()->SetCapsuleSize(110.f, 192.0f);

	spawnDelay = 0.0f;
	maxSpawnDelay = 1.0f;
}