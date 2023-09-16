// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFItem.h"
#include "PFWeaponItem.generated.h"

/**
 * ���� ������ Ŭ����
 * ���� �������� ���� �Ӽ� �� �Լ� ����
 * ���� �������� ����, ����, ������ ĳ��, ����ź�� 4������ ����
 */
UCLASS()
class PF_API APFWeaponItem : public APFItem
{
	GENERATED_BODY()
public:
	APFWeaponItem();

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public :
	// ���� ���������� ����Ǵ� ������ Ŭ���̾�Ʈ�� Character���� �����ϹǷ�, Multicast �Լ� ����
	virtual void MulticastAcquired_Implementation(AActor* actor) override;
	virtual void SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo);
};
