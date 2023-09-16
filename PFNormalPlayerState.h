// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFNormalPlayerState.generated.h"

/**
 * 일반 플레이어 스테이트 클래스
 * 보스 플레이어의 현재 상태 정보 보유
 * 캐릭터 사망 시 관전 상태로 전환, 리스폰 시 관전 상태를 해제
 */
UCLASS()
class PF_API APFNormalPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APFNormalPlayerState(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(class APlayerState* PlayerState) override; // Seamless Travel 수행 시 변수 복제

protected:
	// 캐릭터 속성 변수
	UPROPERTY(Replicated)
	float health;
	UPROPERTY(Replicated)
	float shield;
	UPROPERTY(Replicated)
	uint8 life;
	UPROPERTY(Replicated)
	class APFNormalCharacter* normalCharacter;
	UPROPERTY(Replicated)
	FLinearColor color;
	UPROPERTY(Replicated)
	class APlayerController* normalController;

	const float maxHealth = 100.0f;
	const float maxShield = 80.0f;
	const uint8 maxLife = 5;

	UPROPERTY(Replicated)
	bool bAgreeToStart; // 게임 시작 동의 여부
	UPROPERTY(Replicated)
	bool bIsObserving; // 관전 중인지 여부
	UPROPERTY(Replicated)
	class APlayerController* observingPlayer; // 관전 대상으로 지정된 다른 플레이어
	UPROPERTY(Replicated)
	class APFObserverCamera* observerCamera; // 관전 시점을 제공하는 관전 카메라

	// 상호작용 관련 변수
	UPROPERTY(Replicated)
	bool canInteraction;
	UPROPERTY(Replicated)
	bool onInteraction;

public:
	void ResetState(); // 리스폰 시 변수 초기화

	// 관전 기능을 위한 함수
	void Observing();
	void Unobserving();
	bool IsObserving() const { return bIsObserving; }
	void ChangeObservingTarget();

	void SetNormalCharacter(class APFNormalCharacter* nc);
	class APFNormalCharacter* GetNormalCharacter() const { return normalCharacter; }
	void SetNormalController(class APlayerController* controller);
	class APlayerController* GetNormalController() const { return normalController; }
	void SetObserverCamera(class APFObserverCamera* camera);
	class APFObserverCamera* GetObserverCamera() const { return observerCamera; }
	void SetObservingPlayer(class APlayerController* controller);
	class APlayerController* GetObservingPlayer() const { return observingPlayer; }

	// 상호작용 설정
	void SetAgreeToStart();
	void SetDisagreeToStart();
	bool GetAgreeToStart() const { return bAgreeToStart; }
	// 상호작용 가능 여부 설정
	void SetOnInteraction();
	void SetOffInteraction();
	bool IsOnInteraction() const { return onInteraction; }

	void SetCanInteraction();
	void SetCanNotInteraction();
	bool CanInteraction() const { return canInteraction; }

	void IncreaseHealth(const float& value);
	void DecreaseHealth(const float& value);
	void SetHealth(const float& value);

	void IncreaseShield(const float& value);
	void DecreaseShield(const float& value);
	void SetShield(const float& value);

	void IncreaseLife(const uint8& value);
	void DecreaseLife(const uint8& value);
	void SetLife(const uint8& value);

	void SetColor(const FLinearColor& c);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return health; }
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return maxHealth; }
	UFUNCTION(BlueprintCallable)
	float GetShield() const { return shield; }
	UFUNCTION(BlueprintCallable)
	float GetMaxShield() const { return maxShield; }
	UFUNCTION(BlueprintCallable)
	uint8 GetLife() const { return life; }
	UFUNCTION(BlueprintCallable)
	uint8 GetMaxLife() const { return maxLife; }
	UFUNCTION(BlueprintCallable)
	FLinearColor GetColor() const { return color; }
	UFUNCTION(BlueprintCallable)
	bool IsAgreeToStart() const { return bAgreeToStart; }
};