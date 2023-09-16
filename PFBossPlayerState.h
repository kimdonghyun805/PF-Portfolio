// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFBossPlayerState.generated.h"

/**
 * ���� �÷��̾� ������Ʈ Ŭ����
 * ���� �÷��̾��� ���� ���� ���� ����
 * ĳ���� ��� �� ���� ���·� ��ȯ, ������ �� ���� ���¸� ����
 */
UCLASS()
class PF_API APFBossPlayerState : public APlayerState
{
	GENERATED_BODY()
		
public :
	APFBossPlayerState(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(class APlayerState* PlayerState) override; // Seamless Travel ���� �� ���� ����

protected :
	// ĳ���� �Ӽ� ����
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
	bool bAgreeToStart; // ���� ���� ���� ����
	UPROPERTY(Replicated)
	bool bIsObserving; // ���� ������ ����
	UPROPERTY(Replicated)
	class APlayerController* observingPlayer; // ���� ������� ������ �ٸ� �÷��̾�
	UPROPERTY(Replicated)
	class APFObserverCamera* observerCamera; // ���� ������ �����ϴ� ���� ī�޶�

	// ��ȣ�ۿ� ���� ����
	UPROPERTY(Replicated)
	bool canInteraction;
	UPROPERTY(Replicated)
	bool onInteraction;

public :
	void ResetState(); // ������ �� ���� �ʱ�ȭ

	// ���� ����� ���� �Լ�
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

	// ���� ���� ���Ǹ� ���� �Լ�
	void SetAgreeToStart();
	void SetDisagreeToStart();
	bool GetAgreeToStart() const { return bAgreeToStart; }

	// ��ȣ�ۿ� ����
	void SetOnInteraction();
	void SetOffInteraction();
	bool IsOnInteraction() const { return onInteraction; }
	// ��ȣ�ۿ� ���� ���� ����
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
