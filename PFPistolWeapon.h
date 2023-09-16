// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeapon.h"
#include "PFPistolWeapon.generated.h"

/**
 * 권총 무장 클래스
 * 기본 공격 키를 입력하여 발사
 * 재장전 키를 입력하여 재장전
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
	
	// 서버에 연산 요청
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(const FVector& mouseDir, const FVector& mousePos, const float& pistolDamage, const float& pistolRange);

};
