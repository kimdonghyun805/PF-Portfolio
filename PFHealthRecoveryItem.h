// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFRecoveryItem.h"
#include "PFHealthRecoveryItem.generated.h"

/**
 * ü�� ȸ�� ������ Ŭ����
 * ������ ȹ�� �� �Ϲ� ĳ������ ü�� ȸ��
 */
UCLASS()
class PF_API APFHealthRecoveryItem : public APFRecoveryItem
{
	GENERATED_BODY()
	
public:
	APFHealthRecoveryItem();

private:
	const float recoverHealth = 50.0f; // ���������� ȸ���ϴ� ü��

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(class AActor* actor, const int& itemtype, const float& value) override;

	float GetRecoverHealth() const { return recoverHealth; }
};
