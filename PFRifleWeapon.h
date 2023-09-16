// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeapon.h"
#include "PFRifleWeapon.generated.h"

/**
 * 소총 무장 클래스
 * 기본 공격 키를 입력하여 발사, 기본 공격 키 입력을 유지할 시 자동으로 발사
 * 기본 공격 키를 해제하여 자동 발사 중지
 * 일정량의 탄약을 자동 발사한 경우, 반동으로 인해 발사 방향을 일정량 회전
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

	// 서버에 공격 연산 요청 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnFire(const FVector& mouseDir, const FVector& mousePos, const float& rifleDamage, const float& rifleRange);

protected :
	bool bIsAutoFiring; // 현재 연사 중인 상태
	uint8 numAutoFiredAmmo; // 연사로 발사한 탄약 수
	uint8 numRecoilAmmo; // 연사 시에 반동이 발생하는 탄약 수

public :
	UFUNCTION(BlueprintCallable)
	uint8 GetNumAutoFiredAmmo() const { return numAutoFiredAmmo; }
	UFUNCTION(BlueprintCallable)
	uint8 GetNumRecoilAmmo() const { return numRecoilAmmo; }
};
