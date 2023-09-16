// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFDistrict.h"
#include "PFCollapseDistrict.generated.h"

/**
 * �ر� ���� Ŭ����
 * Ư���� ���������� ���� �޽ÿ� ���� ���࿡ �ʿ��� ���� ������Ʈ ����
 * ������ �ھ� ������Ʈ�� �ı��Ǵ� ��� �ر�
 * �ر��� ������ ������ �� �����ʰ� ���ŵǰ�, �Ϲ� ĳ���Ϳ��� ������ ���ظ� ����
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
	// ������ �󿡼� ������Ʈ �߰� �� ������ �����ϵ��� EditAnywhere �Ӽ� �ο�
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
