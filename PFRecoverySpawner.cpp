// Fill out your copyright notice in the Description page of Project Settings.


#include "PFRecoverySpawner.h"
#include "PFRecoveryItem.h"
#include "PFHealthRecoveryItem.h"
#include "PFShieldRecoveryItem.h"
#include "PFLifeRecoveryItem.h"
#include "Components/StaticMeshComponent.h"

APFRecoverySpawner::APFRecoverySpawner()
{
	// 아이템 생성 쿨타임
	maxSpawnDelay = 30.0f;
	spawnDelay = maxSpawnDelay;

	// 스포너 메시 설정
	//StaticMesh'/Game/PF/PFSpawnPad/Mesh/SM_launchpad_Round.SM_launchpad_Round'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFSpawnPad/Mesh/SM_launchpad_Round.SM_launchpad_Round'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 0.7f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	//Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	reinforceLevel = 0;
	recoveryItemClass.Insert(APFHealthRecoveryItem::StaticClass(), 0);
	recoveryItemClass.Insert(APFShieldRecoveryItem::StaticClass(), 1);
	recoveryItemClass.Insert(APFLifeRecoveryItem::StaticClass(), 2);
}

void APFRecoverySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority && CanSpawn())
	{
		SpawnRecoveryItem(GetRandomRecoveryItemIndex());
	}
}

int APFRecoverySpawner::GetRandomRecoveryItemIndex()
{
	// 강화 단계 별 아이템 등장 확률에 따라 생성할 아이템 선정
	int index = 0;
	int rand = FMath::Rand() % 100;
	if (rand < arr_healthRate[reinforceLevel])
	{
		index = 0;
	}
	else if (rand < arr_healthRate[reinforceLevel] + arr_shieldRate[reinforceLevel])
	{
		index = 1;
	}
	else
	{
		index = 2;
	}

	return index;
}

void APFRecoverySpawner::SpawnRecoveryItem(const int& index)
{
	// 회복 아이템 생성
	if (index < 0 || index > 2)
	{
		return;
	}

	FActorSpawnParameters params;
	FVector loc = GetActorLocation();
	GetWorld()->SpawnActor(recoveryItemClass[index], &loc);

	spawnDelay = maxSpawnDelay;
}


void APFRecoverySpawner::SetReinforceLevel(const int& value)
{
	if (value < 0 || value > 3)
	{
		return;
	}
	reinforceLevel = value;
}

void APFRecoverySpawner::IncreaseReinforceLevel()
{
	++reinforceLevel;
	if (reinforceLevel > 3)
	{
		reinforceLevel = 3;
	}
	else if (reinforceLevel < 0)
	{
		reinforceLevel = 0;
	}
}