// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFCharacter.h"
#include "PFBossCharacter.generated.h"

/**
 * ���� ĳ���� Ŭ����
 * �Ϲ� ĳ���Ϳ� ����
 * ���� �ھ� ������Ʈ�� �ı�
 * �⺻ ����, ���� ��ų, ���� ��ų ���
 * �ھ �ı��� �� ���� �ɷ�ġ�� ��ȭ
 * ���� ������ �� ȸ�� ������ ȹ�� ����
 */
UCLASS()
class PF_API APFBossCharacter : public APFCharacter
{
	GENERATED_BODY()
	
public :
	APFBossCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override; // �Է� ���ε�
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void OnPrimaryAction() override; // �⺻ ���� Ű �Է� �� ����
	virtual void Landed(const FHitResult& Hit) override; // ���� �� ���Ͻ� ����
	virtual void MoveBlockedBy(const FHitResult& Impact) override; // �̵� �� �浹 �߻� �� ����

public :
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventIntigator, AActor* DamageCauser) override;
	virtual void PossessedBy(AController* NewController) override; // ���ο� ��Ʈ�ѷ� ����
	virtual void FellOutOfWorld(const class UDamageType& dmgType); // ���� ������ ������ �� ȣ��

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // ��Ʈ��ũ ���� ����

protected :
	class APFBossPlayerState* BossPlayerState;

public:
	class APFBossPlayerState* GetBossPlayerState();
	void SetBossPlayerState(class APFBossPlayerState * newPS);

public:
	// ���� �ִϸ��̼� ��Ÿ��
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	TArray<class UAnimMontage*> AttackAnimMontage;

	// ���� ����
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* AttackSound;
	// ���� ����
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* ChargeSound;
	// ����� ����
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* ShockwaveSound;

	// ��ȭ ȿ�� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ReinforceNiagara;
	// ���� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* AttackNiagara;
	// ����� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ShockwaveNiagara;
	// ���� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* ChargeNiagara;

protected :
	// ���콺 ���� ��� �Լ�
	FVector GetMouseDirection();
	
	// ���� ��ų ���� �Լ�
	void StartJumpKey();
	void EndJumpKey();
	// ���� ��ų ���� �Լ�
	void StartChargeKey();
	void EndChargeKey();

	// ���� ���� ��ü ���� �Լ�
	TArray<AActor*> SweepSphere(const float& radius, const FVector& start, const FVector& end);
	// ��ų ��� �� ���� Ÿ�� ���� �Լ�
	void Crash(const FHitResult& Hit, const float& damage);
	// ��ų ��� �� ����� ���ؿ� �˹� ���� �Լ�
	void ShockWave(bool activateEffect, const FRotator& rotation, const float& damage, const float& range, const float& jumpCharge, bool isCharging);
	// �Ϲ� ���� �Լ�
	void BasicAttack(const FVector& mouseDir, const FRotator& rotation, const float& damage, const float& range, const float& radius);

public :
	void BossKilled(bool canRespawn); // ���� ĳ���� ���
	void BossRespawn(); // ������ �ϰ� ĳ���� ����
	void BossRagdollDestroy(); // ������ ���� �ʰ� ĳ���� ����

	// ĳ������ �ɷ�ġ�� ��ȭ
	UFUNCTION(Server, Reliable)
	void ServerReinforce(const uint8& level);
	UFUNCTION(NetMulticast, Reliable)
	void Reinforce(const uint8& level);
	UFUNCTION(Server, Reliable)
	void ReinforceNext();

	// ������ ���� ���� ��û �Լ�
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrash(const FHitResult& Hit, const float& damage);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShockWave(bool activateEffect, const FRotator& rotation, const float& damage, const float& range, const float& jumpCharge, bool isCharging);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBasicAttack(const FVector& mouseDir, const FRotator& rotation, const float& damage, const float& range, const float& radius);

	// �ִϸ��̼� ��� �Լ�
	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackAnim();

	// ����Ʈ ��� �Լ�
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
	// ���� ��ų ���� ����
	float chargeTime; // ���� ���� ��ų ���� �ð� 
	float chargeMaxTime; // ���� ��ų �ִ� ���� �ð�
	bool bIsCharging; // ���� ���� �������� ǥ��
	float chargeSpeed; // ���� �ӵ� ����
	FVector chargeDirection; // ���� ����
	const float chargeWalkSpeed = 2000.0f; // ���� �� �����ϴ� �̵� �ӵ�
	float chargeCoolDown; // ���� ���� ��ų ��Ÿ��
	float chargeMaxCoolDown; // ���� ��ų �ִ� ��Ÿ��
	FVector chargeEffectScale; // ���� ����Ʈ ũ��

	// ���� ��ų ���� ����
	float jumpChargeValue; // ���� ���� ��ų ������
	const float jumpMaxChargeValue = 800.0f; // ���� ��ų �ִ� ������
	bool bIsJumpCharging; // ���� ���� ��ų ���������� ǥ��
	float jumpChargePerSec; // �ʴ� �����Ǵ� ���� ������
	float jumpCoolDown; // ���� ���� ��ų ��Ÿ��
	float jumpMaxCoolDown; // ���� ��ų �ִ� ��Ÿ��

	// ����� ���� ����
	float crashDamage; // ���� �浹�� ���ϴ� ����
	float shockDamage; // ����� ����
	//const float shockRange = 1000.0f;
	float shockRange; // ����� ����
	FVector shockEffectScale; // ����� ����Ʈ ũ��

	// �Ϲ� ���� ���� ����
	float attackDamage; // �Ϲ� ���� ����
	float attackRange; // �Ϲ� ���� ���� �Ÿ�
	const float attackRadius = 300.0f; // �Ϲ� ���� �ݰ�
	float attackDelay; // ���� �Ϲ� ���� ������
	float attackMaxDelay; // �Ϲ� ���� ������ �ִ�
	FVector attackEffectScale; // �Ϲ� ���� ����Ʈ ũ��

	// ��Ÿ ���� ����
	float defaultJumpVelocity; // ���� ĳ������ �⺻ ������
	float defaultWalkSpeed; // ���� ĳ������ �⺻ �̵� �ӵ�
	float armor; // ����
	TArray<class UMaterial*> reinforceMaterial; // ��ȭ ȿ�� ��Ƽ����
	int numAttack; // ���� �ִϸ��̼��� �����ϴ� ����
	const float respawnTime = 10.0f;
	FTimerHandle respawnTimerHandle;

	// �ھ� �ı��� ���� ��ȭ�Ǵ� �ɷ�ġ�� �迭
	const float arr_chargeMaxTime[4] = { 1.2f, 1.2f, 1.4f, 1.4f };
	const float arr_chargeSpeed[4] = { 400.0f, 600.0f, 800.0f, 1000.0f };
	const float arr_chargeMaxCoolDown[4] = { 6.0f, 5.5f, 5.0f, 4.5f };
	const float arr_chargeEffectMag[4] = { 8.0f, 10.0f, 13.0f, 15.0f };

	const float arr_jumpChargePerSec[4] = { 600.0f, 600.0f, 600.0f, 600.0f };
	const float arr_jumpMaxCoolDown[4] = { 7.0f, 6.5f, 6.0f, 5.5f };

	const float arr_crashDamage[4] = { 50.0f, 50.0f, 60.0f, 60.0f };

	const float arr_shockDamage[4] = { 20.0f, 20.0f, 25.0f, 30.0f };
	const float arr_shockRange[4] = { 1000.0f, 1200.0f, 1400.0f, 1600.0f };
	const float arr_shockEffectMag[4] = { 20.0f, 24.0f, 28.0f, 32.0f }; // ����� ����Ʈ ũ�� ����

	const float arr_attackDamage[4] = { 10.0f, 12.0f, 14.0f, 15.0f };
	const float arr_attackRange[4] = { 500.0f, 1000.0f, 2000.0f, 4000.0f };
	const float arr_attackEffectMag[4] = { 1.6f, 2.6f, 4.5f, 9.0f }; // ���� ����Ʈ �⺻ ���� 500�� ���� �����Ÿ��� ����
	const float arr_attackMaxDelay[4] = { 0.7f, 0.6f, 0.5f, 0.4f };

	const float arr_defaultJumpVelocity[4] = { 580.0f, 600.0f, 640.0f, 680.0f };
	const float arr_defaultWalkSpeed[4] = { 680.0f, 730.0f, 780.0f, 830.0f };
	const float arr_armor[4] = { 0.0f, 2.0f, 4.0f, 6.0f };

public :
	// ������Ʈ ��ȣ�ۿ� �Լ�
	UFUNCTION(Server, Reliable)
	void SetOnInteraction();
	UFUNCTION(Server, Reliable)
	void SetOffInteraction();
	bool IsOnInteraction();

	// ĳ���� ���� ���� �Լ�
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
