// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFRecoveryItem.h"
#include "PFShieldRecoveryItem.generated.h"

/**
 * 보호막 회복 아이템 클래스
 * 아이템 획득 시 일반 캐릭터의 보호막 수치 증가
 */
UCLASS()
class PF_API APFShieldRecoveryItem : public APFRecoveryItem
{
	GENERATED_BODY()

public:
	APFShieldRecoveryItem();

private:
	const float recoverShield = 40.0f; // 아이템이 공급하는 장탄 수

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(class AActor* actor, const int& itemtype, const float& value) override;

	float GetRecoverShield() const { return recoverShield; }
};
