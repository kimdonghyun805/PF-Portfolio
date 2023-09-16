// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFItem.h"
#include "PFRecoveryItem.generated.h"

/**
 * ȸ�� ������ Ŭ����
 * ȸ�� �������� ���� �Ӽ� �� �Լ� ����
 * ȸ�� �������� ü��, ��ȣ��, ����� 3������ ����
 */
UCLASS()
class PF_API APFRecoveryItem : public APFItem
{
	GENERATED_BODY()
	
public:
	APFRecoveryItem();

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	// ȸ�� ���������� ����Ǵ� ������ PlayerState���� �����ϹǷ�, Server �Լ� ����
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(AActor* actor, const int& itemtype, const float& value);
};
