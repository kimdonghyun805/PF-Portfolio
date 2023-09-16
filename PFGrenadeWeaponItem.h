// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeaponItem.h"
#include "PFGrenadeWeaponItem.generated.h"

/**
 * ����ź ���� ������ Ŭ����
 * ������ ȹ�� �� �Ϲ� ĳ������ ����ź ���忡 ��ź ����
 */
UCLASS()
class PF_API APFGrenadeWeaponItem : public APFWeaponItem
{
	GENERATED_BODY()

public:
	APFGrenadeWeaponItem();

private:
	const int supplyingAmmo = 5; // �������� �����ϴ� ��ź ��

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void MulticastAcquired_Implementation(AActor* actor) override;
	virtual void SupplyAmmo(class AActor* actor, const int& weaponindex, const int& ammo) override;

	int GetSupplyingAmmo() const { return supplyingAmmo; }
};
