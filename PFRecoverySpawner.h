// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFRecoverySpawner.generated.h"

/**
 * ȸ�� ������ ������ Ŭ����
 * ���� �ð� ���� ȸ�� �������� ��ġ���� ȸ�� �������� ����
 * �ھ �ı��� �� ���� ��ȭ �ܰ谡 ����, ��ȭ �ܰ迡 ���� ������ ���� Ȯ�� ����
 */
UCLASS()
class PF_API APFRecoverySpawner : public APFSpawner
{
	GENERATED_BODY()
public :
	APFRecoverySpawner();

	virtual void Tick(float DeltaTime) override;

protected:
	TArray<TSubclassOf<class APFRecoveryItem>> recoveryItemClass; // ������ ȸ�� ������ ����

	int reinforceLevel; // ��ȭ �ܰ�

	// ��ȭ �ܰ迡 ���� ȸ�� ������ ���� Ȯ��
	const int arr_healthRate[4] = { 50, 50, 50, 50 };
	const int arr_shieldRate[4] = {  0, 20, 40, 50 };
	const int arr_lifeRate[4] =   { 50, 30, 10,  0 };

protected:
	int GetRandomRecoveryItemIndex();
	void SpawnRecoveryItem(const int& index);

public:
	void SetReinforceLevel(const int& value);
	void IncreaseReinforceLevel();
};
