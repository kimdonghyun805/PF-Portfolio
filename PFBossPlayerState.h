// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFBossPlayerState.generated.h"

/**
 * 보스 플레이어 스테이트 클래스
 * 보스 플레이어의 현재 상태 정보 보유
 * 캐릭터 사망 시 관전 상태로 전환, 리스폰 시 관전 상태를 해제
 */
UCLASS()
class PF_API APFBossPlayerState : public APlayerState
{
	GENERATED_BODY()
		
public :
	APFBossPlayerState(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(class APlayerState* PlayerState) override; // Seamless Travel 수행 시 변수 복제

protected :
	// 캐릭터 속성 변수
	UPROPERTY(Replicated)
	float health;
	UPROPERTY(Replicated)
	uint8 reinforceLevel;
	UPROPERTY(Replicated)
	class APFBossCharacter* bossCharacter;
	UPROPERTY(Replicated)
	class APlayerController* bossController;

	const float maxHealth = 6000.0f;

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

public :
	void ResetState(); // 리스폰 시 변수 초기화

	// 관전 기능을 위한 함수
	void Observing();
	void Unobserving();
	bool IsObserving() const { return bIsObserving; }
	void ChangeObservingTarget();

	void SetBossCharacter(APFBossCharacter* bc);
	APFBossCharacter* GetBossCharacter() const { return bossCharacter; }
	void SetBossController(APlayerController* bc);
	class APlayerController* GetBossController() const { return bossController; }
	void SetObserverCamera(class APFObserverCamera* camera);
	class APFObserverCamera* GetObserverCamera() const { return observerCamera; }
	void SetObservingPlayer(class APlayerController* controller);
	class APlayerController* GetObservingPlayer() const { return observingPlayer; }

	// 게임 시작 동의를 위한 함수
	void SetAgreeToStart();
	void SetDisagreeToStart();
	bool GetAgreeToStart() const { return bAgreeToStart; }

	// 상호작용 설정
	void SetOnInteraction();
	void SetOffInteraction();
	bool IsOnInteraction() const { return onInteraction; }
	// 상호작용 가능 여부 설정
	void SetCanInteraction();
	void SetCanNotInteraction();
	bool CanInteraction() const { return canInteraction; }

	void IncreaseReinforceLevel();
	void DecreaseReinforceLevel();
	void SetReinforceLevel(const uint8& value);

	void IncreaseHealth(const float& value);
	void DecreaseHealth(const float& value);
	void SetHealth(const float& value);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return health; }
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return maxHealth; }
	UFUNCTION(BlueprintCallable)
	uint8 GetReinforceLevel() const { return reinforceLevel; }
	UFUNCTION(BlueprintCallable)
	bool IsAgreeToStart() const { return bAgreeToStart; }
};
