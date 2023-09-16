// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeaponItem.h"
#include "PFLasorCannonWeaponItem.generated.h"

/**
 * ������ ĳ�� ���� ������ Ŭ����
 * ������ ȹ�� �� �Ϲ� ĳ������ ������ ĳ�� ���忡 ��ź ����
 */
UCLASS()
class PF_API APFLasorCannonWeaponItem : public APFWeaponItem
{
	GENERATED_BODY()
	
public:
	APFLasorCannonWeaponItem();

private:
	const int supplyingAmmo = 50; // �������� �����ϴ� ��ź ��

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void MulticastAcquired_Implementation(AActor* actor) override;
	virtual void SupplyAmmo(class AActor* actor, const int& weaponindex, const int& ammo) override;

	int GetSupplyingAmmo() const { return supplyingAmmo; }
};
