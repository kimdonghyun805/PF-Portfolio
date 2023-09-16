// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "PFWeapon.generated.h"

/**
 * ���� Ŭ����
 * ������ ���� �Ӽ� �� �Լ� ����
 * ������ ����, ����, ������ ĳ��, ����ź�� 4������ ����
 */
UCLASS()
class PF_API UPFWeapon : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPFWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected :
	class APFNormalCharacter* weaponOwner; // ���� ���� �Ϲ� ĳ����

	bool bIsPicked; // ������ ���� ���� ����
	float swapDelay; // ���� ��ü ������
	float maxSwapDelay; // �ִ� ���� ��ü ������
	FVector swapMovement; // ���� ��ü ������ ���� 1��Ī �޽��� ������
	FVector meshLocation1P; // 1��Ī �޽� ��ġ

	float range; // ������ �����Ÿ�
	float damage; // ������ ����

	uint8 remainAmmo; // ������ ��ź �� - ����
	uint8 maxRemainAmmo; // �ִ�� ������ �� �ִ� ��ź ��

	uint8 loadedAmmo; // ���� ������ ��ź �� - ����
	uint8 maxLoadedAmmo; // �ִ�� ������ �� �ִ� ��ź ��

	bool bIsReloading; // ���� ���� ���� - ����
	float remainReloadingTime; // ���� �ð� - ����
	float maxReloadingTime; // �ִ� ���� �ð�

	float fireDelay; // �߻� ������
	float maxFireDelay; // �ִ� �߻� ������

protected :
	// 1��Ī ���� �޽�
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;
	// 3��Ī ���� �޽�
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

	// �޽� ����
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterial3P;
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterial1P;

	// �߻� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UParticleSystemComponent* FireParticle3P;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UParticleSystemComponent* FireParticle1P;
	// �Ϲ� ĳ���� �ִϸ��̼� ��Ÿ��
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* CharacterReloadAnimMontage;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* CharacterFireAnimMontage;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* CharacterEquipAnimMontage;
	// ���� �ִϸ��̼� ��Ÿ��
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* WeaponReloadAnimMontage;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* WeaponFireAnimMontage;

public :
	virtual void OnFire(const FVector& mouseDir, const FVector& mousePos); // �⺻ ���� Ű �Է� �� ����
	virtual void AfterFire(const FVector& mouseDir, const FVector& mousePos); // �⺻ ���� Ű ���� �� ����
	virtual void Reload(); // ������ ����
	virtual void LoadAmmo(); // ��ź ����

	AActor* LineTrace(const FVector& start, const FVector& end); // �߻� �� ����Ʈ���̽� ����

	// ���� ���� �� ���� Ȱ��ȭ�� ���� ���� ���� ����
	UFUNCTION(Server, Reliable)
	virtual void ServerPickThisWeapon();
	virtual void PickThisWeapon();
	// ���� ���� �� ���� ��Ȱ��ȭ�� ���� ���� ���� ����
	UFUNCTION(Server, Reliable)
	virtual void ServerUnpickThisWeapon();
	virtual void UnpickThisWeapon();

	// ���� �ִϸ��̼� ��û
	UFUNCTION(Server, Unreliable)
	void ServerPlayReloadAnim();
	UFUNCTION(Server, Unreliable)
	void ServerPlayFireAnim();
	UFUNCTION(Server, Unreliable)
	void ServerPlayEquipAnim();

	// �ִϸ��̼� ��Ƽĳ��Ʈ
	UFUNCTION(NetMulticast, Unreliable)
	void PlayReloadAnim();
	UFUNCTION(NetMulticast, Unreliable)
	void PlayFireAnim();
	UFUNCTION(NetMulticast, Unreliable)
	void PlayEquipAnim();

public :
	UFUNCTION(NetMulticast, Reliable)
	virtual void SetWeaponColor(const FLinearColor& color);

	UFUNCTION(Server, Reliable)
	void ServerSetWeaponOwner(class APFNormalCharacter* owner);
	UFUNCTION(NetMulticast, Reliable)
	void SetWeaponOwner(class APFNormalCharacter* owner);
	UFUNCTION(BlueprintCallable)
	APFNormalCharacter* GetWeaponOwner() const { return weaponOwner; }

	void SetRemainAmmo(const uint8& value);
	void IncreaseRemainAmmo(const uint8& value);
	UFUNCTION(BlueprintCallable)
	uint8 GetRemainAmmo() const { return remainAmmo; }
	UFUNCTION(BlueprintCallable)
	uint8 GetMaxRemainAmmo() const { return maxRemainAmmo; }

	void SetLoadedAmmo(const uint8& value);
	void IncreaseLoadedAmmo(const uint8& value);
	UFUNCTION(BlueprintCallable)
	uint8 GetLoadedAmmo() const { return loadedAmmo; }
	UFUNCTION(BlueprintCallable)
	uint8 GetMaxLoadedAmmo() const { return maxLoadedAmmo; }

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	UFUNCTION(BlueprintCallable)
	float GetRemainReloadingTime() const { return remainReloadingTime; }
	UFUNCTION(BlueprintCallable)
	float GetMaxReloadingTime() const { return maxReloadingTime; }
	UFUNCTION(BlueprintCallable)
	float GetFireDelay() const { return fireDelay; }
	UFUNCTION(BlueprintCallable)
	float GetMaxFireDelay() const { return maxFireDelay; }
	UFUNCTION(BlueprintCallable)
	bool GetIsPicked() const { return bIsPicked; }

	void SetSwapDelay(const float& value);
	UFUNCTION(BlueprintCallable)
	float GetSwapDelay() const { return swapDelay; }
	UFUNCTION(BlueprintCallable)
	float GetMaxSwapDelay() const { return maxSwapDelay; }

	UFUNCTION(BlueprintCallable)
	bool IsAmmoEmpty();
};
