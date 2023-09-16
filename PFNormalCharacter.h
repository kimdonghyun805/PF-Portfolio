// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFCharacter.h"
#include "PFNormalCharacter.generated.h"

/**
 * �Ϲ� ĳ���� Ŭ����
 * ������ ����, ������ ����Ͽ� ���� ĳ���Ϳ� ����
 * ���� ������ �� ȸ�� ������ ȹ�� ����
 */
UCLASS()
class PF_API APFNormalCharacter : public APFCharacter
{
	GENERATED_BODY()

public:
	APFNormalCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void OnPrimaryAction() override; // �⺻ ���� Ű �Է� �� ����
	void AfterPrimaryAction(); // �⺻ ���� Ű ���� �� ����

public :
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventIntigator, AActor* DamageCauser) override;
	// ���ο� ��Ʈ�ѷ� ���� �Լ�
	virtual void PossessedBy(AController* NewController) override;
	virtual void FellOutOfWorld(const class UDamageType& dmgType); // ���� ������ ������ �� ȣ��

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// �÷��̾� ��Ʈ�ѷ��� �ʿ��� �÷��̾� ������Ʈ
	class APFNormalPlayerState* NormalPlayerState;

public:
	// �÷��̾� ������Ʈ�� ���� ������ �Լ�
	class APFNormalPlayerState* GetNormalPlayerState();
	void SetNormalPlayerState(class APFNormalPlayerState* newPS);

public :
	// ���콺 ����� ��ġ ��� �Լ�
	TArray<FVector> GetMouseDirectionAndPosition();
	
protected :
	const float defaultJumpVelocity = 420.0f; // �Ϲ� ĳ������ �⺻ ������
	const float defaultWalkSpeed = 600.0f; // �Ϲ� ĳ������ �⺻ �̵� �ӵ�
	const float respawnTime = 10.0f; // �Ϲ� ĳ������ ������ ��� �ð�
	// ������ Ÿ�̸� �ڵ�
	FTimerHandle respawnTimerHandle;

	// �÷��̾� �޽� ����
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterialBottom;
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterialTop;

	// ��� ���� ������Ʈ �迭
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TArray<class UPFWeapon*> weapons;

	// ���� ������� ����
	class UPFWeapon* holdingWeapon; // ���� ������� ���� ������
	int32 holdingWeaponIndex; // ���� ������� ���� �ε���

protected :
	void SelectWeapon(const int32& number); // Ư�� ��ȣ�� ���� ����
	// Ư�� ���� ����
	void SelectPistolWeapon();
	void SelectRifleWeapon();
	void SelectLasorCannonWeapon();
	void SelectGrenadeWeapon();

	void Reload(); // ������ Ű �Է� �� ����
	// ���� ��ü �� ����
	UFUNCTION(Server, Reliable)
	void ServerPickWeapon(const int32& number);
	UFUNCTION(NetMulticast, Reliable)
	void PickWeapon(const int32& number);
	UFUNCTION(Server, Reliable)
	void ServerUnpickWeapon(const int32& number);
	UFUNCTION(NetMulticast, Reliable)
	void UnpickWeapon(const int32& number);

public:
	void NormalKilled(bool canRespawn);
	void NormalRespawn();
	void NormalRagdollDestroy();

	void SelectNextWeapon(float value); // ���콺 �� �Է� ������ ���� ������ ���� ���� ����

	// �޽� ���� ����
	void SetNormalColor(const FLinearColor& color);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetNormalColor(const FLinearColor& color);
	UFUNCTION(Client, Reliable)
	void ClientSetNormalColor(const FLinearColor& color);
	FLinearColor GetNormalColor() const;

	// ��ȣ�ۿ� ���� ����
	UFUNCTION(Server, Reliable)
	void SetOnInteraction();
	UFUNCTION(Server, Reliable)
	void SetOffInteraction();
	bool IsOnInteraction();

	// ĳ���� ���� ����
	UFUNCTION(Server, Reliable)
	void SetHealth(const float& value);
	UFUNCTION(Server, Reliable)
	void DecreaseHealth(const float& value);
	UFUNCTION(Server, Reliable)
	void IncreaseHealth(const float& value);

	UFUNCTION(Server, Reliable)
	void SetShield(const float& value);
	UFUNCTION(Server, Reliable)
	void DecreaseShield(const float& value);
	UFUNCTION(Server, Reliable)
	void IncreaseShield(const float& value);

	UFUNCTION(Server, Reliable)
	void SetLife(const uint8& value);
	UFUNCTION(Server, Reliable)
	void DecreaseLife(const uint8& value);
	UFUNCTION(Server, Reliable)
	void IncreaseLife(const uint8& value);

public :
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetShield() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxShield() const;
	UFUNCTION(BlueprintCallable)
	uint8 GetLife() const;
	UFUNCTION(BlueprintCallable)
	uint8 GetMaxLife() const;
	UPFWeapon* GetHoldingWeapon() const { return holdingWeapon; }
	UFUNCTION(BlueprintCallable)
	int32 GetHoldingWeaponIndex() const { return holdingWeaponIndex; }
	UFUNCTION(BlueprintCallable)
	TArray<class UPFWeapon*> GetWeapons() const { return weapons; }
	UFUNCTION(BlueprintCallable)
	class UPFWeapon* GetPistolWeapon() const { return weapons[0]; }
	UFUNCTION(BlueprintCallable)
	class UPFWeapon* GetRifleWeapon() const { return weapons[1]; }
	UFUNCTION(BlueprintCallable)
	class UPFWeapon* GetLasorCannonWeapon() const { return weapons[2]; }
	UFUNCTION(BlueprintCallable)
	class UPFWeapon* GetGrenadeWeapon() const { return weapons[3]; }

};
