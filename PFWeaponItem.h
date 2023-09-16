// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFItem.h"
#include "PFWeaponItem.generated.h"

/**
 * 무장 아이템 클래스
 * 무장 아이템의 공통 속성 및 함수 설정
 * 무장 아이템은 권총, 소총, 레이저 캐논, 수류탄의 4종류로 구성
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
	// 무장 아이템으로 변경되는 변수는 클라이언트의 Character에서 보유하므로, Multicast 함수 실행
	virtual void MulticastAcquired_Implementation(AActor* actor) override;
	virtual void SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo);
};
