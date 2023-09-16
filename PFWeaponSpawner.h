// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFWeaponSpawner.generated.h"

/**
 * 무장 아이템 스포너 클래스
 * 일정 시간 마다 무장 스포너의 위치에서 무장 아이템을 생성
 * 코어가 파괴될 때 마다 강화 단계가 변경, 강화 단계에 따라 아이템 생성 확률 변동
 */
UCLASS()
class PF_API APFWeaponSpawner : public APFSpawner
{
	GENERATED_BODY()
public :
	APFWeaponSpawner();

	virtual void Tick(float DeltaTime) override;

protected :
	TArray<TSubclassOf<class APFWeaponItem>> weaponItemClass; // 생성될 무장 아이템 종류

	int reinforceLevel; // 강화 단계

	// 강화 단계에 따른 회복 아이템 등장 확률
	const int arr_pistolRate[4] =		{ 25, 20,  0,  0 };
	const int arr_rifleRate[4] =		{ 50, 50, 50, 25 };
	const int arr_lasorCannonRate[4] =	{  0,  5, 25, 50 };
	const int arr_grenadeRate[4] =		{ 25, 25, 25, 25 };

protected :
	int GetRandomWeaponItemIndex();
	void SpawnWeaponItem(const int& index);

public :
	void SetReinforceLevel(const int& value);
	void IncreaseReinforceLevel();
};
