// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeapon.h"
#include "PFGrenadeWeapon.generated.h"

/**
 * ����ź ���� Ŭ����
 * �⺻ ���� Ű�� �Է��Ͽ� ����ź ��ô
 * ������ �Ұ���
 */
UCLASS()
class PF_API UPFGrenadeWeapon : public UPFWeapon
{
	GENERATED_BODY()
	
public:
	UPFGrenadeWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void OnFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void AfterFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void Reload() override;
	virtual void LoadAmmo() override;

	void Fire(const FVector& spawnLoc, const FRotator& spawnRot);

	// ������ ���� ���� ��û �Լ�
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(const FVector& spawnLoc, const FRotator& spawnRot);

protected :
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* projSpawn;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APFProjectile> projClass;
};
