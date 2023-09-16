// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFRecoveryItem.h"
#include "PFHealthRecoveryItem.generated.h"

/**
 * 체력 회복 아이템 클래스
 * 아이템 획득 시 일반 캐릭터의 체력 회복
 */
UCLASS()
class PF_API APFHealthRecoveryItem : public APFRecoveryItem
{
	GENERATED_BODY()
	
public:
	APFHealthRecoveryItem();

private:
	const float recoverHealth = 50.0f; // 아이템으로 회복하는 체력

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(class AActor* actor, const int& itemtype, const float& value) override;

	float GetRecoverHealth() const { return recoverHealth; }
};
