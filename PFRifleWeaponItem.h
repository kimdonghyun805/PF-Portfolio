// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeaponItem.h"
#include "PFRifleWeaponItem.generated.h"

/**
 * ���� ���� ������ Ŭ����
 * ������ ȹ�� �� �Ϲ� ĳ������ ���� ���忡 ��ź ����
 */
UCLASS()
class PF_API APFRifleWeaponItem : public APFWeaponItem
{
	GENERATED_BODY()
public:
	APFRifleWeaponItem();

private:
	const int supplyingAmmo = 60; // �������� �����ϴ� ��ź ��

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void MulticastAcquired_Implementation(AActor* actor) override;
	virtual void SupplyAmmo(class AActor* actor, const int& weaponindex, const int& ammo) override;

	int GetSupplyingAmmo() const { return supplyingAmmo; }
};
