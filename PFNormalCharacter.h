// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFCharacter.h"
#include "PFNormalCharacter.generated.h"

/**
 * 일반 캐릭터 클래스
 * 무장을 보유, 무장을 사용하여 보스 캐릭터에 대항
 * 무장 아이템 및 회복 아이템 획득 가능
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

	virtual void OnPrimaryAction() override; // 기본 공격 키 입력 시 실행
	void AfterPrimaryAction(); // 기본 공격 키 종료 시 실행

public :
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventIntigator, AActor* DamageCauser) override;
	// 새로운 컨트롤러 적용 함수
	virtual void PossessedBy(AController* NewController) override;
	virtual void FellOutOfWorld(const class UDamageType& dmgType); // 월드 밖으로 떨어질 때 호출

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// 플레이어 컨트롤러에 필요한 플레이어 스테이트
	class APFNormalPlayerState* NormalPlayerState;

public:
	// 플레이어 스테이트에 대한 접근자 함수
	class APFNormalPlayerState* GetNormalPlayerState();
	void SetNormalPlayerState(class APFNormalPlayerState* newPS);

public :
	// 마우스 방향과 위치 계산 함수
	TArray<FVector> GetMouseDirectionAndPosition();
	
protected :
	const float defaultJumpVelocity = 420.0f; // 일반 캐릭터의 기본 점프력
	const float defaultWalkSpeed = 600.0f; // 일반 캐릭터의 기본 이동 속도
	const float respawnTime = 10.0f; // 일반 캐릭터의 리스폰 대기 시간
	// 리스폰 타이머 핸들
	FTimerHandle respawnTimerHandle;

	// 플레이어 메시 색상
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterialBottom;
	UPROPERTY(VisibleAnywhere, Category = Material)
	class UMaterialInstanceDynamic* dynamicMaterialTop;

	// 모든 무장 컴포넌트 배열
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TArray<class UPFWeapon*> weapons;

	// 현재 사용중인 무장
	class UPFWeapon* holdingWeapon; // 현재 사용중인 무장 포인터
	int32 holdingWeaponIndex; // 현재 사용중인 무장 인덱스

protected :
	void SelectWeapon(const int32& number); // 특정 번호의 무장 선택
	// 특정 무장 선택
	void SelectPistolWeapon();
	void SelectRifleWeapon();
	void SelectLasorCannonWeapon();
	void SelectGrenadeWeapon();

	void Reload(); // 재장전 키 입력 시 실행
	// 무장 교체 시 실행
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

	void SelectNextWeapon(float value); // 마우스 휠 입력 등으로 현재 무장의 다음 무장 선택

	// 메시 색상 설정
	void SetNormalColor(const FLinearColor& color);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetNormalColor(const FLinearColor& color);
	UFUNCTION(Client, Reliable)
	void ClientSetNormalColor(const FLinearColor& color);
	FLinearColor GetNormalColor() const;

	// 상호작용 여부 설정
	UFUNCTION(Server, Reliable)
	void SetOnInteraction();
	UFUNCTION(Server, Reliable)
	void SetOffInteraction();
	bool IsOnInteraction();

	// 캐릭터 변수 설정
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
