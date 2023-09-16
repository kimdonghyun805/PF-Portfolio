// Fill out your copyright notice in the Description page of Project Settings.


#include "PFWeaponSpawner.h"
#include "PFWeaponItem.h"
#include "PFPistolWeaponItem.h"
#include "PFRifleWeaponItem.h"
#include "PFLasorCannonWeaponItem.h"
#include "PFGrenadeWeaponItem.h"
#include "Components/StaticMeshComponent.h"

APFWeaponSpawner::APFWeaponSpawner()
{
	// 아이템 생성 쿨타임
	maxSpawnDelay = 20.0f;
	spawnDelay = 0.0f; // 게임 시작과 동시에 아이템 생성

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
	weaponItemClass.Insert(APFPistolWeaponItem::StaticClass(), 0);
	weaponItemClass.Insert(APFRifleWeaponItem::StaticClass(), 1);
	weaponItemClass.Insert(APFLasorCannonWeaponItem::StaticClass(), 2);
	weaponItemClass.Insert(APFGrenadeWeaponItem::StaticClass(), 3);
}

void APFWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority && CanSpawn())
	{
		SpawnWeaponItem(GetRandomWeaponItemIndex());
	}
}

int APFWeaponSpawner::GetRandomWeaponItemIndex()
{
	// 강화 단계 별 아이템 등장 확률에 따라 생성할 아이템 선정
	int index = 0;
	int rand = FMath::Rand() % 100;
	if (rand < arr_pistolRate[reinforceLevel])
	{
		index = 0;
	}
	else if (rand < arr_pistolRate[reinforceLevel] + arr_rifleRate[reinforceLevel])
	{
		index = 1;
	}
	else if (rand < arr_pistolRate[reinforceLevel] + arr_rifleRate[reinforceLevel] + arr_lasorCannonRate[reinforceLevel])
	{
		index = 2;
	}
	else
	{
		index = 3;
	}
	
	return index;
}

void APFWeaponSpawner::SpawnWeaponItem(const int& index)
{
	// 무장 아이템 생성
	if (index < 0 || index > 3)
	{
		return;
	}

	FActorSpawnParameters params;
	FVector loc = GetActorLocation();
	GetWorld()->SpawnActor(weaponItemClass[index], &loc);

	spawnDelay = maxSpawnDelay;
}

void APFWeaponSpawner::SetReinforceLevel(const int& value)
{
	if (value < 0 || value > 3)
	{
		return;
	}
	reinforceLevel = value;
}

void APFWeaponSpawner::IncreaseReinforceLevel()
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