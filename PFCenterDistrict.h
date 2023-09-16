// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFDistrict.h"
#include "PFCenterDistrict.generated.h"

/**
 * �߾� ���� Ŭ����
 * ���� ���� �� ���� ĳ���Ͱ� �����Ǵ� ����
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

	// ������ �󿡼� ������Ʈ �߰� �� ������ �����ϵ��� EditAnywhere �Ӽ� �ο�
	UPROPERTY(EditAnywhere, Category = District)
	TArray<class APFBossSpawner*> BossSpawners;

public :
	TArray<class APFBossSpawner*> GetBossSpawners() const { return BossSpawners; }
};
