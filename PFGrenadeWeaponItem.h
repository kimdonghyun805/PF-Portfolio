// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeaponItem.h"
#include "PFGrenadeWeaponItem.generated.h"

/**
 * 수류탄 무장 아이템 클래스
 * 아이템 획득 시 일반 캐릭터의 수류탄 무장에 장탄 공급
 */
UCLASS()
class PF_API APFGrenadeWeaponItem : public APFWeaponItem
{
	GENERATED_BODY()

public:
	APFGrenadeWeaponItem();

private:
	const int supplyingAmmo = 5; // 아이템이 공급하는 장탄 수

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void MulticastAcquired_Implementation(AActor* actor) override;
	virtual void SupplyAmmo(class AActor* actor, const int& weaponindex, const int& ammo) override;

	int GetSupplyingAmmo() const { return supplyingAmmo; }
};
