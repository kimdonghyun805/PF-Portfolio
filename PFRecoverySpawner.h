// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFRecoverySpawner.generated.h"

/**
 * 회복 아이템 스포너 클래스
 * 일정 시간 마다 회복 스포너의 위치에서 회복 아이템을 생성
 * 코어가 파괴될 때 마다 강화 단계가 변경, 강화 단계에 따라 아이템 생성 확률 변동
 */
UCLASS()
class PF_API APFRecoverySpawner : public APFSpawner
{
	GENERATED_BODY()
public :
	APFRecoverySpawner();

	virtual void Tick(float DeltaTime) override;

protected:
	TArray<TSubclassOf<class APFRecoveryItem>> recoveryItemClass; // 생성될 회복 아이템 종류

	int reinforceLevel; // 강화 단계

	// 강화 단계에 따른 회복 아이템 등장 확률
	const int arr_healthRate[4] = { 50, 50, 50, 50 };
	const int arr_shieldRate[4] = {  0, 20, 40, 50 };
	const int arr_lifeRate[4] =   { 50, 30, 10,  0 };

protected:
	int GetRandomRecoveryItemIndex();
	void SpawnRecoveryItem(const int& index);

public:
	void SetReinforceLevel(const int& value);
	void IncreaseReinforceLevel();
};
