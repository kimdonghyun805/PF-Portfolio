// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFRecoveryItem.h"
#include "PFShieldRecoveryItem.generated.h"

/**
 * ��ȣ�� ȸ�� ������ Ŭ����
 * ������ ȹ�� �� �Ϲ� ĳ������ ��ȣ�� ��ġ ����
 */
UCLASS()
class PF_API APFShieldRecoveryItem : public APFRecoveryItem
{
	GENERATED_BODY()

public:
	APFShieldRecoveryItem();

private:
	const float recoverShield = 40.0f; // �������� �����ϴ� ��ź ��

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(class AActor* actor, const int& itemtype, const float& value) override;

	float GetRecoverShield() const { return recoverShield; }
};
