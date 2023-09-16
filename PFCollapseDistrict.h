// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFDistrict.h"
#include "PFCollapseDistrict.generated.h"

/**
 * 붕괴 구역 클래스
 * 특수한 지형지물을 가진 메시와 게임 진행에 필요한 각종 오브젝트 보유
 * 구역의 코어 오브젝트가 파괴되는 경우 붕괴
 * 붕괴된 구역은 아이템 및 스포너가 제거되고, 일반 캐릭터에게 지속적 피해를 입힘
 */
UCLASS()
class PF_API APFCollapseDistrict : public APFDistrict
{
	GENERATED_BODY()
public :
	APFCollapseDistrict();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected :
	// 에디터 상에서 오브젝트 추가 및 수정이 가능하도록 EditAnywhere 속성 부여
	UPROPERTY(EditAnywhere, Category = District)
	TArray<class APFNormalSpawner*> NormalSpawners;
	UPROPERTY(EditAnywhere, Category = District)
	TArray<class APFWeaponSpawner*> WeaponSpawners;
	UPROPERTY(EditAnywhere, Category = District)
	TArray<class APFRecoverySpawner*> RecoverySpawners;
	UPROPERTY(EditAnywhere, Category = District)
	class APFCoreObject* CoreObject;
	UPROPERTY(EditAnywhere, Category = District)
	TArray<class APFParticleSpawner*> ParticleSpawners;

	bool bIsCollapsed;
	const float tickDamage = 5.0f;

	float collapsingTime;
	const float maxCollapsingTime = 60.0f;
	float damageTime;
	const float maxDamageTime = 1.0f;
	float normalSpawnerDestroyTime;
	const float maxNormalSpawnerDestroyTime = 2.0f;
	float itemSpawnerDestroyTime;
	const float maxItemSpawnerDestroyTime = 10.0f;

	bool bIsItemDestroyed;

protected :
	void DestroyRandomNormalSpawner();
	void DestroyRandomWeaponSpawner();
	void DestroyRandomRecoverySpawner();

public :
	void CoreDestroyed();
	void Collapse();

	UFUNCTION(BlueprintCallable)
	bool IsCollapsed() const { return bIsCollapsed; }
	TArray<class APFNormalSpawner*> GetNormalSpawners() const { return NormalSpawners; }
	TArray<class APFWeaponSpawner*> GetWeaponSpawners() const { return WeaponSpawners; }
	TArray<class APFRecoverySpawner*> GetRecoverySpawners() const { return RecoverySpawners; }
	UFUNCTION(BlueprintCallable)
	class APFCoreObject* GetCoreObject() const { return CoreObject; }
};
