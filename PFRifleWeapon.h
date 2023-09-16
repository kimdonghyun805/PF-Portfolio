// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeapon.h"
#include "PFRifleWeapon.generated.h"

/**
 * ���� ���� Ŭ����
 * �⺻ ���� Ű�� �Է��Ͽ� �߻�, �⺻ ���� Ű �Է��� ������ �� �ڵ����� �߻�
 * �⺻ ���� Ű�� �����Ͽ� �ڵ� �߻� ����
 * �������� ź���� �ڵ� �߻��� ���, �ݵ����� ���� �߻� ������ ������ ȸ��
 */
UCLASS()
class PF_API UPFRifleWeapon : public UPFWeapon
{
	GENERATED_BODY()

public:
	UPFRifleWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void OnFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void AfterFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void Reload() override;
	virtual void LoadAmmo() override;
	virtual void UnpickThisWeapon() override;

	void Fire(const FVector& mouseDir, const FVector& mousePos, const float& rifleDamage, const float& rifleRange);

	// ������ ���� ���� ��û �Լ�
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(const FVector& mouseDir, const FVector& mousePos, const float& rifleDamage, const float& rifleRange);

protected :
	bool bIsAutoFiring; // ���� ���� ���� ����
	uint8 numAutoFiredAmmo; // ����� �߻��� ź�� ��
	uint8 numRecoilAmmo; // ���� �ÿ� �ݵ��� �߻��ϴ� ź�� ��

public :
	UFUNCTION(BlueprintCallable)
	uint8 GetNumAutoFiredAmmo() const { return numAutoFiredAmmo; }
	UFUNCTION(BlueprintCallable)
	uint8 GetNumRecoilAmmo() const { return numRecoilAmmo; }
};
