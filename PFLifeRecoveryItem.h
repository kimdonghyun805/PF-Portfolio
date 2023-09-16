// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFRecoveryItem.h"
#include "PFLifeRecoveryItem.generated.h"

/**
 * 목숨 회복 아이템 클래스
 * 아이템 획득 시 일반 캐릭터의 목숨 증가
 */
UCLASS()
class PF_API APFLifeRecoveryItem : public APFRecoveryItem
{
	GENERATED_BODY()

public:
	APFLifeRecoveryItem();

private:
	const int recoverLife = 1; // 아이템이 공급하는 장탄 수

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(class AActor* actor, const int& itemtype, const float& value) override;

	int GetRecoverLife() const { return recoverLife; }
};
