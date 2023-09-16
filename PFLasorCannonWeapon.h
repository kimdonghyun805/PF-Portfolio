// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFWeapon.h"
#include "PFLasorCannonWeapon.generated.h"

/**
 * 레이저 캐논 무장 클래스
 * 기본 공격 키를 입력하여 레이저 충전
 * 기본 공격 키를 해제하여 레이저 발사
 * 재장전 불가능
 */
UCLASS()
class PF_API UPFLasorCannonWeapon : public UPFWeapon
{
	GENERATED_BODY()

public:
	UPFLasorCannonWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected :
	bool bIsCharging; // 현재 충전 중인 상태
	float chargeTime; // 충전 시간
	float radius; // 레이저 기본 반지름

	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ChargeParticle1P;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ChargeParticle3P;

	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* LasorNiagara;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* LasorSound;

protected :
	virtual void OnFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void AfterFire(const FVector& mouseDir, const FVector& mousePos) override;
	virtual void Reload() override;
	virtual void LoadAmmo() override;
	
	virtual void UnpickThisWeapon() override;

	void Fire(const FVector& mouseDir, const FVector& mousePos, const float& lasorDamage, const float& lasorRange, const float& lasorRadius, const uint8& ammo);

	UFUNCTION(Server, Reliable)
	void ServerPlayChargeEffect(bool on);
	UFUNCTION(NetMulticast, Reliable)
	void PlayChargeEffect(bool on);
	UFUNCTION(NetMulticast, Unreliable)
	void PlayLasorEffect(const FVector& start, const FVector& end, const float& lasorRadius);

	// 서버에 공격 연산 요청 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAfterFire(const FVector& mouseDir, const FVector& mousePos, const float& lasorDamage, const float& lasorRange, const float& lasorRadius, const uint8& ammo);

public :
	UFUNCTION(BlueprintCallable)
	float GetChargeTime() const { return chargeTime; }
};
