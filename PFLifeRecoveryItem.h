// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFRecoveryItem.h"
#include "PFLifeRecoveryItem.generated.h"

/**
 * ��� ȸ�� ������ Ŭ����
 * ������ ȹ�� �� �Ϲ� ĳ������ ��� ����
 */
UCLASS()
class PF_API APFLifeRecoveryItem : public APFRecoveryItem
{
	GENERATED_BODY()

public:
	APFLifeRecoveryItem();

private:
	const int recoverLife = 1; // �������� �����ϴ� ��ź ��

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(class AActor* actor, const int& itemtype, const float& value) override;

	int GetRecoverLife() const { return recoverLife; }
};
