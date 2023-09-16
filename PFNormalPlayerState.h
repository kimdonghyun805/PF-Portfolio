// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PFNormalPlayerState.generated.h"

/**
 * �Ϲ� �÷��̾� ������Ʈ Ŭ����
 * ���� �÷��̾��� ���� ���� ���� ����
 * ĳ���� ��� �� ���� ���·� ��ȯ, ������ �� ���� ���¸� ����
 */
UCLASS()
class PF_API APFNormalPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APFNormalPlayerState(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(class APlayerState* PlayerState) override; // Seamless Travel ���� �� ���� ����

protected:
	// ĳ���� �Ӽ� ����
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

public:
	void ResetState(); // ������ �� ���� �ʱ�ȭ

	// ���� ����� ���� �Լ�
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

	// ��ȣ�ۿ� ����
	void SetAgreeToStart();
	void SetDisagreeToStart();
	bool GetAgreeToStart() const { return bAgreeToStart; }
	// ��ȣ�ۿ� ���� ���� ����
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