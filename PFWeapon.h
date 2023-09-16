// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "PFWeapon.generated.h"

/**
 * 무장 클래스
 * 무장의 공통 속성 및 함수 설정
 * 무장은 권총, 소총, 레이저 캐논, 수류탄의 4종류로 구성
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
	class APFNormalCharacter* weaponOwner; // 무장 소유 일반 캐릭터

	bool bIsPicked; // 무장이 선택 중인 상태
	float swapDelay; // 무장 교체 딜레이
	float maxSwapDelay; // 최대 무장 교체 딜레이
	FVector swapMovement; // 무장 교체 연출을 위한 1인칭 메시의 움직임
	FVector meshLocation1P; // 1인칭 메시 위치

	float range; // 무장의 사정거리
	float damage; // 무장의 피해

	uint8 remainAmmo; // 보유한 장탄 수 - 변동
	uint8 maxRemainAmmo; // 최대로 보유할 수 있는 장탄 수

	uint8 loadedAmmo; // 장전 상태의 장탄 수 - 변동
	uint8 maxLoadedAmmo; // 최대로 장전할 수 있는 장탄 수

	bool bIsReloading; // 장전 중인 상태 - 변동
	float remainReloadingTime; // 장전 시간 - 변동
	float maxReloadingTime; // 최대 장전 시간

	float fireDelay; // 발사 딜레이
	float maxFireDelay; // 최대 발사 딜레이

protected :
	// 1인칭 무장 메시
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;
	// 3인칭 무장 메시
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

	// 메시 색상
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterial3P;
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterial1P;

	// 발사 이펙트
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UParticleSystemComponent* FireParticle3P;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UParticleSystemComponent* FireParticle1P;
	// 일반 캐릭터 애니메이션 몽타주
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* CharacterReloadAnimMontage;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* CharacterFireAnimMontage;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* CharacterEquipAnimMontage;
	// 무장 애니메이션 몽타주
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* WeaponReloadAnimMontage;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UAnimMontage* WeaponFireAnimMontage;

public :
	virtual void OnFire(const FVector& mouseDir, const FVector& mousePos); // 기본 공격 키 입력 시 실행
	virtual void AfterFire(const FVector& mouseDir, const FVector& mousePos); // 기본 공격 키 해제 시 실행
	virtual void Reload(); // 재장전 수행
	virtual void LoadAmmo(); // 장탄 공급

	AActor* LineTrace(const FVector& start, const FVector& end); // 발사 시 라인트레이스 수행

	// 무장 선택 시 무장 활성화를 위한 각종 변수 설정
	UFUNCTION(Server, Reliable)
	virtual void ServerPickThisWeapon();
	virtual void PickThisWeapon();
	// 무장 해제 시 무장 비활성화를 위한 각종 변수 설정
	UFUNCTION(Server, Reliable)
	virtual void ServerUnpickThisWeapon();
	virtual void UnpickThisWeapon();

	// 서버 애니메이션 요청
	UFUNCTION(Server, Unreliable)
	void ServerPlayReloadAnim();
	UFUNCTION(Server, Unreliable)
	void ServerPlayFireAnim();
	UFUNCTION(Server, Unreliable)
	void ServerPlayEquipAnim();

	// 애니메이션 멀티캐스트
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
