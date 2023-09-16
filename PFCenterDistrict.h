// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFDistrict.h"
#include "PFCenterDistrict.generated.h"

/**
 * 중앙 구역 클래스
 * 게임 시작 시 보스 캐릭터가 스폰되는 구역
 */
UCLASS()
class PF_API APFCenterDistrict : public APFDistrict
{
	GENERATED_BODY()
public:
	APFCenterDistrict();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected :
	float healTime;
	const float maxHealTime = 1.0f;
	const float healingHealth = 50.0f;

	// 에디터 상에서 오브젝트 추가 및 수정이 가능하도록 EditAnywhere 속성 부여
	UPROPERTY(EditAnywhere, Category = District)
	TArray<class APFBossSpawner*> BossSpawners;

public :
	TArray<class APFBossSpawner*> GetBossSpawners() const { return BossSpawners; }
};
