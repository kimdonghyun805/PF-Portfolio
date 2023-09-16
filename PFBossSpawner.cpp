// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBossSpawner.h"
#include "Components/CapsuleComponent.h"

APFBossSpawner::APFBossSpawner()
{
	// ���� ĳ������ ���� ��ġ ����
	GetCapsuleComponent()->SetCapsuleSize(110.f, 192.0f);

	spawnDelay = 0.0f;
	maxSpawnDelay = 1.0f;
}