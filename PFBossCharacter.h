// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFCharacter.h"
#include "PFBossCharacter.generated.h"

/**
 * 보스 캐릭터 클래스
 * 일반 캐릭터에 대항
 * 맵의 코어 오브젝트를 파괴
 * 기본 공격, 돌진 스킬, 점프 스킬 사용
 * 코어를 파괴할 때 마다 능력치가 강화
 * 무장 아이템 및 회복 아이템 획득 가능
 */
UCLASS()
class PF_API APFBossCharacter : public APFCharacter
{
	GENERATED_BODY()
	
public :
	APFBossCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override; // 입력 바인딩
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void OnPrimaryAction() override; // 기본 공격 키 입력 시 실행
	virtual void Landed(const FHitResult& Hit) override; // 점프 후 낙하시 실행
	virtual void MoveBlockedBy(const FHitResult& Impact) override; // 이동 중 충돌 발생 시 실행

public :
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventIntigator, AActor* DamageCauser) override;
	virtual void PossessedBy(AController* NewController) override; // 새로운 컨트롤러 적용
	virtual void FellOutOfWorld(const class UDamageType& dmgType); // 월드 밖으로 떨어질 때 호출

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // 네트워크 변수 복제

protected :
	class APFBossPlayerState* BossPlayerState;

public:
	class APFBossPlayerState* GetBossPlayerState();
	void SetBossPlayerState(class APFBossPlayerState * newPS);

public:
	// 공격 애니메이션 몽타주
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	TArray<class UAnimMontage*> AttackAnimMontage;

	// 공격 사운드
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* AttackSound;
	// 돌진 사운드
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* ChargeSound;
	// 충격파 사운드
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* ShockwaveSound;

	// 강화 효과 이펙트
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ReinforceNiagara;
	// 공격 이펙트
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* AttackNiagara;
	// 충격파 이펙트
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ShockwaveNiagara;
	// 돌진 이펙트
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ChargeNiagara;

protected :
	// 마우스 방향 계산 함수
	FVector GetMouseDirection();
	
	// 점프 스킬 관련 함수
	void StartJumpKey();
	void EndJumpKey();
	// 돌진 스킬 관련 함수
	void StartChargeKey();
	void EndChargeKey();

	// 일정 범위 구체 스윕 함수
	TArray<AActor*> SweepSphere(const float& radius, const FVector& start, const FVector& end);
	// 스킬 사용 시 직접 타격 피해 함수
	void Crash(const FHitResult& Hit, const float& damage);
	// 스킬 사용 시 충격파 피해와 넉백 실행 함수
	void ShockWave(bool activateEffect, const FRotator& rotation, const float& damage, const float& range, const float& jumpCharge, bool isCharging);
	// 일반 공격 함수
	void BasicAttack(const FVector& mouseDir, const FRotator& rotation, const float& damage, const float& range, const float& radius);

public :
	void BossKilled(bool canRespawn); // 보스 캐릭터 사망
	void BossRespawn(); // 리스폰 하고 캐릭터 제거
	void BossRagdollDestroy(); // 리스폰 하지 않고 캐릭터 제거

	// 캐릭터의 능력치를 강화
	UFUNCTION(Server, Reliable)
	void ServerReinforce(const uint8& level);
	UFUNCTION(NetMulticast, Reliable)
	void Reinforce(const uint8& level);
	UFUNCTION(Server, Reliable)
	void ReinforceNext();

	// 서버에 공격 연산 요청 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrash(const FHitResult& Hit, const float& damage);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShockWave(bool activateEffect, const FRotator& rotation, const float& damage, const float& range, const float& jumpCharge, bool isCharging);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBasicAttack(const FVector& mouseDir, const FRotator& rotation, const float& damage, const float& range, const float& radius);

	// 애니메이션 재생 함수
	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackAnim();

	// 이펙트 재생 함수
	UFUNCTION(NetMulticast, Unreliable)
	void PlayReinforceEffect();
	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackEffect(const FRotator& rotation);
	UFUNCTION(NetMulticast, Unreliable)
	void PlayShockwaveEffect(const FVector& location);
	UFUNCTION(NetMulticast, Unreliable)
	void PlayChargeEffect(const FVector& location, const FRotator& rotation);

	UFUNCTION(Server, Reliable)
	void ServerLaunchCharacter(const FVector& dir);
	UFUNCTION(Server, Reliable)
	void ServerSetChargeVariable(bool on, const FVector& direction, const float& walkSpeedCharge, const float& walkSpeedDefault);
	UFUNCTION(NetMulticast, Reliable)
	void SetChargeVariable(bool on, const FVector& direction, const float& walkSpeedCharge, const float& walkSpeedDefault);
	UFUNCTION(Server, Reliable)
	void ServerSetJumpVariable(bool on, const float& jumpCharge, const float& jumpVelocityDefault);
	UFUNCTION(NetMulticast, Reliable)
	void SetJumpVariable(bool on, const float& jumpCharge, const float& jumpVelocityDefault);

protected :
	// 돌진 스킬 관련 변수
	float chargeTime; // 현재 돌진 스킬 시전 시간 
	float chargeMaxTime; // 돌진 스킬 최대 시전 시간
	bool bIsCharging; // 현재 돌진 상태임을 표시
	float chargeSpeed; // 돌진 속도 배율
	FVector chargeDirection; // 돌진 방향
	const float chargeWalkSpeed = 2000.0f; // 돌진 시 증가하는 이동 속도
	float chargeCoolDown; // 현재 돌진 스킬 쿨타임
	float chargeMaxCoolDown; // 돌진 스킬 최대 쿨타임
	FVector chargeEffectScale; // 돌진 이펙트 크기

	// 점프 스킬 관련 변수
	float jumpChargeValue; // 현재 점프 스킬 충전량
	const float jumpMaxChargeValue = 800.0f; // 점프 스킬 최대 충전량
	bool bIsJumpCharging; // 현재 점프 스킬 충전중임을 표시
	float jumpChargePerSec; // 초당 충전되는 점프 충전량
	float jumpCoolDown; // 현재 점프 스킬 쿨타임
	float jumpMaxCoolDown; // 점프 스킬 최대 쿨타임

	// 충격파 관련 변수
	float crashDamage; // 직접 충돌시 가하는 피해
	float shockDamage; // 충격파 피해
	//const float shockRange = 1000.0f;
	float shockRange; // 충격파 범위
	FVector shockEffectScale; // 충격파 이펙트 크기

	// 일반 공격 관련 변수
	float attackDamage; // 일반 공격 피해
	float attackRange; // 일반 공격 사정 거리
	const float attackRadius = 300.0f; // 일반 공격 반경
	float attackDelay; // 현재 일반 공격 딜레이
	float attackMaxDelay; // 일반 공격 딜레이 최대
	FVector attackEffectScale; // 일반 공격 이펙트 크기

	// 기타 상태 변수
	float defaultJumpVelocity; // 보스 캐릭터의 기본 점프력
	float defaultWalkSpeed; // 보스 캐릭터의 기본 이동 속도
	float armor; // 방어력
	TArray<class UMaterial*> reinforceMaterial; // 강화 효과 머티리얼
	int numAttack; // 공격 애니메이션을 선택하는 변수
	const float respawnTime = 10.0f;
	FTimerHandle respawnTimerHandle;

	// 코어 파괴에 따라 강화되는 능력치의 배열
	const float arr_chargeMaxTime[4] = { 1.2f, 1.2f, 1.4f, 1.4f };
	const float arr_chargeSpeed[4] = { 400.0f, 600.0f, 800.0f, 1000.0f };
	const float arr_chargeMaxCoolDown[4] = { 6.0f, 5.5f, 5.0f, 4.5f };
	const float arr_chargeEffectMag[4] = { 8.0f, 10.0f, 13.0f, 15.0f };

	const float arr_jumpChargePerSec[4] = { 600.0f, 600.0f, 600.0f, 600.0f };
	const float arr_jumpMaxCoolDown[4] = { 7.0f, 6.5f, 6.0f, 5.5f };

	const float arr_crashDamage[4] = { 50.0f, 50.0f, 60.0f, 60.0f };

	const float arr_shockDamage[4] = { 20.0f, 20.0f, 25.0f, 30.0f };
	const float arr_shockRange[4] = { 1000.0f, 1200.0f, 1400.0f, 1600.0f };
	const float arr_shockEffectMag[4] = { 20.0f, 24.0f, 28.0f, 32.0f }; // 충격파 이펙트 크기 배율

	const float arr_attackDamage[4] = { 10.0f, 12.0f, 14.0f, 15.0f };
	const float arr_attackRange[4] = { 500.0f, 1000.0f, 2000.0f, 4000.0f };
	const float arr_attackEffectMag[4] = { 1.6f, 2.6f, 4.5f, 9.0f }; // 공격 이펙트 기본 길이 500에 대한 사정거리의 배율
	const float arr_attackMaxDelay[4] = { 0.7f, 0.6f, 0.5f, 0.4f };

	const float arr_defaultJumpVelocity[4] = { 580.0f, 600.0f, 640.0f, 680.0f };
	const float arr_defaultWalkSpeed[4] = { 680.0f, 730.0f, 780.0f, 830.0f };
	const float arr_armor[4] = { 0.0f, 2.0f, 4.0f, 6.0f };

public :
	// 오브젝트 상호작용 함수
	UFUNCTION(Server, Reliable)
	void SetOnInteraction();
	UFUNCTION(Server, Reliable)
	void SetOffInteraction();
	bool IsOnInteraction();

	// 캐릭터 변수 관련 함수
	UFUNCTION(Server, Reliable)
	void SetHealth(const float& value);
	UFUNCTION(Server, Reliable)
	void DecreaseHealth(const float& value);
	UFUNCTION(Server, Reliable)
	void IncreaseHealth(const float& value);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	uint8 GetReinforceLevel() const;
	UFUNCTION(BlueprintCallable)
	float GetArmor() const { return armor; }

	UFUNCTION(BlueprintCallable)
	float GetJumpCoolDown() const { return jumpCoolDown; }
	UFUNCTION(BlueprintCallable)
	float GetJumpMaxCoolDown() const { return jumpMaxCoolDown; }
	UFUNCTION(BlueprintCallable)
	bool IsJumpCharging() const { return bIsJumpCharging; }
	UFUNCTION(BlueprintCallable)
	float GetJumpChargeValue() const { return jumpChargeValue; }
	UFUNCTION(BlueprintCallable)
	float GetJumpMaxChargeValue() const { return jumpMaxChargeValue; }

	FVector GetChargeDirection() const { return chargeDirection; }
	bool IsCharging() const { return bIsCharging; }
	UFUNCTION(BlueprintCallable)
	float GetChargeCoolDown() const { return chargeCoolDown; }
	UFUNCTION(BlueprintCallable)
	float GetChargeMaxCoolDown() const { return chargeMaxCoolDown; }
	UFUNCTION(BlueprintCallable)
	float GetChargeTime() const { return chargeTime; }
	UFUNCTION(BlueprintCallable)
	float GetChargeMaxTime() const { return chargeMaxTime; }

	UFUNCTION(BlueprintCallable)
	float GetAttackDelay() const { return attackDelay; }
	UFUNCTION(BlueprintCallable)
	float GetAttackMaxDelay() const { return attackMaxDelay; }
};
