// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFWeaponSpawner.generated.h"

/**
 * ���� ������ ������ Ŭ����
 * ���� �ð� ���� ���� �������� ��ġ���� ���� �������� ����
 * �ھ �ı��� �� ���� ��ȭ �ܰ谡 ����, ��ȭ �ܰ迡 ���� ������ ���� Ȯ�� ����
 */
UCLASS()
class PF_API APFWeaponSpawner : public APFSpawner
{
	GENERATED_BODY()
public :
	APFWeaponSpawner();

	virtual void Tick(float DeltaTime) override;

protected :
	TArray<TSubclassOf<class APFWeaponItem>> weaponItemClass; // ������ ���� ������ ����

	int reinforceLevel; // ��ȭ �ܰ�

	// ��ȭ �ܰ迡 ���� ȸ�� ������ ���� Ȯ��
	const int arr_pistolRate[4] =		{ 25, 20,  0,  0 };
	const int arr_rifleRate[4] =		{ 50, 50, 50, 25 };
	const int arr_lasorCannonRate[4] =	{  0,  5, 25, 50 };
	const int arr_grenadeRate[4] =		{ 25, 25, 25, 25 };

protected :
	int GetRandomWeaponItemIndex();
	void SpawnWeaponItem(const int& index);

public :
	void SetReinforceLevel(const int& value);
	void IncreaseReinforceLevel();
};
