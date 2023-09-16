// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "PFGameMode.generated.h"

/**
 * ���� ��� Ŭ����
 * ���� ���࿡ �ʿ��� �������� ���� ����
 * ���ӿ� �����ϴ� �÷��̾� ���� �� ��ȯ
 * �� ������Ʈ�� Ž���Ͽ� ���� ���࿡ �ʿ��� ������Ʈ ����
 * �÷��̾�� ������Ʈ�� ��ȣ�ۿ� ����
 * SeamlessTravel ������� �� ��ȯ (��� �� <-> ���� ��)
 * ������ ����, ����, ����
 */
UCLASS(minimalapi)
class APFGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APFGameMode();
protected :
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Logout(class AController* Exiting) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(class APlayerController* NewPlayer) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//virtual void PostSeamlessTravel() override;
	virtual void InitSeamlessTravelPlayer(class AController* NewController) override;
	virtual void HandleSeamlessTravelPlayer(class AController*& NewController) override;
	//virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	//virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

protected :
	uint8 numNormalCharacter; // ���� �������� �Ϲ� ĳ���� ��
	uint8 numBossCharacter; // ���� �������� ���� ĳ���� ��

	TArray<class APlayerController*> Players; // ���� �÷��̾� �迭
	TArray<class APlayerController*> ObservingPlayers; // ���� ������ �÷��̾�
	TArray<class APlayerController*> PlayingPlayers; // ���� ���� �ƴ� �÷��̾�
	TArray<class APlayerController*> AgreeToStartPlayers; // ���� ���ۿ� ������ �÷��̾�

	TArray<class APFObserverCamera*> ObserverCameras; // ���� ���� �÷��̾��� ���� ī�޶�

	TArray<class APFCollapseDistrict*> CollapseDistricts; // �ر� ����
	TArray<class APFCenterDistrict*> CenterDistricts; // �߾� ���� �迭

	TArray<FLinearColor> NormalColors; // �Ϲ� ĳ���Ϳ� ����Ǵ� ����
	TArray<FLinearColor> AvailableNormalColors; // ��� ������ �Ϲ� ĳ������ ����

	FTimerHandle gameStartTimerHandle;
	const float gameStartDelay = 5.0f;
	FTimerHandle gameEndTimerHandle;
	const float gameEndDelay = 5.0f;
	FTimerHandle timeOverTimerHandle;
	float gameTime;
	const float maxGameTime = 480.0f;

	uint8 numDestroyedCore;
	uint8 numKilledBoss;
	uint8 numKilledNormal;
	GameResult gameResult;

	// Seamless Travel�� ���� ���� �ÿ��� �����Ǿ�� �ϴ� �����͸� static���� ����
	static bool bIsGameStart; // ���� ���� ���� Ȯ��
	static APlayerController* bossPlayer; // ���� �÷��̾��� ��Ʈ�ѷ�

public :
	void ChangeNormalToBoss(class APlayerController* controller); // ĳ���� ���� �Լ�
	void ChangeBossToNormal(class APlayerController* controller);

	void NormalRespawn(class APlayerController* controller);
	void BossRespawn(class APlayerController* controller);

	void NormalSpawn(class APFNormalCharacter* normalCharacter);
	void BossSpawn(class APFBossCharacter* bossCharacter);

	void CoreDestroyed(class APFCoreObject* core);
	void BossFinished(class APFBossPlayerState* bossPS);
	void NormalFinished(class APFNormalPlayerState* normalPS);

	void FindCollapseDistrict(); // ���� �ر� ���� Ž��
	void FindCenterDistrict(); // ���� �߾� ���� Ž��

	void ChangeObservingTargetAboutThisPlayer(class APlayerController* controller); // ���ڷ� �޴� �÷��̾ ���� ���� ī�޶��� ���� ��� ����
	TArray<class APlayerController*> GetObservingTargetPlayer(); // ���� ������ ��� Ž��
	TArray<class APFObserverCamera*> GetDistrictObserverCamera(); // ������ ī�޶� Ž��

	void StartGame(); // ���� ���� -> ���� ������ �̵�
	void EndGame(); // ���� ���� -> ��� ������ �̵�
	void TimeOver(); // ���� �ð� ����

	void AddPlayer(class APlayerController* controller);
	void RemovePlayer(class APlayerController* controller);
	void AddObservingPlayer(class APlayerController* controller);
	void RemoveObservingPlayer(class APlayerController* controller);
	void AddPlayingPlayer(class APlayerController* controller);
	void RemovePlayingPlayer(class APlayerController* controller);
	void AddAgreeToStartPlayer(class APlayerController* controller);
	void RemoveAgreeToStartPlayer(class APlayerController* controller);

	void AddObserverCamera(class APFObserverCamera* camera);
	void RemoveObserverCamera(class APFObserverCamera* camera);

public :
	UFUNCTION(BlueprintCallable)
	TArray<class APlayerController*> GetPlayers() const { return Players; }
	UFUNCTION(BlueprintCallable)
	TArray<class APlayerController*> GetPlayingPlayers() const { return PlayingPlayers; }
	UFUNCTION(BlueprintCallable)
	TArray<class APlayerController*> GetObservingPlayers() const { return ObservingPlayers; }
	UFUNCTION(BlueprintCallable)
	TArray<class APFCollapseDistrict*> GetCollapseDistricts() const { return CollapseDistricts; }
	UFUNCTION(BlueprintCallable)
	TArray<class APFCenterDistrict*> GetCenterDistricts() const { return CenterDistricts; }
	UFUNCTION(BlueprintCallable)
	bool IsGameStart() const { return bIsGameStart; }
};
// static ���� �ʱ�ȭ
bool APFGameMode::bIsGameStart = false;
APlayerController* APFGameMode::bossPlayer = nullptr;

// ���� ����� ��Ÿ���� Enum Ŭ����
UENUM(BlueprintType)
enum class GameResult : uint8
{
	GameNotEnd UMETA(DisplayName = "GameNotEnd"),
	AllCoreDestroyed UMETA(DisplayName = "AllCoreDestroyed"),
	AllNormalFinished UMETA(DisplayName = "AllNormalFinished"),
	BossFinished UMETA(DisplayName = "BossFinished"),
	TimeOver UMETA(DisplayName = "TimeOver")
};
