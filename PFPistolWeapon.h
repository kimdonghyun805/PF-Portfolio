// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeapon.h"
#include "PFPistolWeapon.generated.h"

/**
 * ���� ���� Ŭ����
 * �⺻ ���� Ű�� �Է��Ͽ� �߻�
 * ������ Ű�� �Է��Ͽ� ������
 */
UCLASS()
class PF_API UPFPistolWeapon : public UPFWeapon
{
	GENERATED_BODY()

public:
	UPFPistolWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void OnFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void AfterFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void Reload() override;
	virtual void LoadAmmo() override;

	void Fire(const FVector& mouseDir, const FVector& mousePos, const float& pistolDamage, const float& pistolRange);
	
	// ������ ���� ��û
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(const FVector& mouseDir, const FVector& mousePos, const float& pistolDamage, const float& pistolRange);

};
