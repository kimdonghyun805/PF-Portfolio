// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "PFGameMode.generated.h"

/**
 * 게임 모드 클래스
 * 게임 진행에 필요한 전반적인 정보 보유
 * 게임에 참여하는 플레이어 검증 및 소환
 * 맵 오브젝트를 탐색하여 게임 진행에 필요한 오브젝트 수집
 * 플레이어와 오브젝트의 상호작용 관리
 * SeamlessTravel 방식으로 맵 전환 (대기 맵 <-> 게임 맵)
 * 게임을 실행, 진행, 종료
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
	uint8 numNormalCharacter; // 게임 참여중인 일반 캐릭터 수
	uint8 numBossCharacter; // 게임 참여중인 보스 캐릭터 수

	TArray<class APlayerController*> Players; // 참가 플레이어 배열
	TArray<class APlayerController*> ObservingPlayers; // 관전 상태의 플레이어
	TArray<class APlayerController*> PlayingPlayers; // 관전 중이 아닌 플레이어
	TArray<class APlayerController*> AgreeToStartPlayers; // 게임 시작에 동의한 플레이어

	TArray<class APFObserverCamera*> ObserverCameras; // 관전 중인 플레이어의 관전 카메라

	TArray<class APFCollapseDistrict*> CollapseDistricts; // 붕괴 구역
	TArray<class APFCenterDistrict*> CenterDistricts; // 중앙 구역 배열

	TArray<FLinearColor> NormalColors; // 일반 캐릭터에 적용되는 색상
	TArray<FLinearColor> AvailableNormalColors; // 사용 가능한 일반 캐릭터의 색상

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

	// Seamless Travel로 레벨 변경 시에도 유지되어야 하는 데이터를 static으로 선언
	static bool bIsGameStart; // 게임 시작 여부 확인
	static APlayerController* bossPlayer; // 보스 플레이어의 컨트롤러

public :
	void ChangeNormalToBoss(class APlayerController* controller); // 캐릭터 변경 함수
	void ChangeBossToNormal(class APlayerController* controller);

	void NormalRespawn(class APlayerController* controller);
	void BossRespawn(class APlayerController* controller);

	void NormalSpawn(class APFNormalCharacter* normalCharacter);
	void BossSpawn(class APFBossCharacter* bossCharacter);

	void CoreDestroyed(class APFCoreObject* core);
	void BossFinished(class APFBossPlayerState* bossPS);
	void NormalFinished(class APFNormalPlayerState* normalPS);

	void FindCollapseDistrict(); // 맵의 붕괴 구역 탐색
	void FindCenterDistrict(); // 맵의 중앙 구역 탐색

	void ChangeObservingTargetAboutThisPlayer(class APlayerController* controller); // 인자로 받는 플레이어를 관전 중인 카메라의 관전 대상 변경
	TArray<class APlayerController*> GetObservingTargetPlayer(); // 관전 가능한 대상 탐색
	TArray<class APFObserverCamera*> GetDistrictObserverCamera(); // 구역의 카메라 탐색

	void StartGame(); // 게임 시작 -> 게임 맵으로 이동
	void EndGame(); // 게임 종료 -> 대기 맵으로 이동
	void TimeOver(); // 제한 시간 종료

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
// static 변수 초기화
bool APFGameMode::bIsGameStart = false;
APlayerController* APFGameMode::bossPlayer = nullptr;

// 게임 결과를 나타내는 Enum 클래스
UENUM(BlueprintType)
enum class GameResult : uint8
{
	GameNotEnd UMETA(DisplayName = "GameNotEnd"),
	AllCoreDestroyed UMETA(DisplayName = "AllCoreDestroyed"),
	AllNormalFinished UMETA(DisplayName = "AllNormalFinished"),
	BossFinished UMETA(DisplayName = "BossFinished"),
	TimeOver UMETA(DisplayName = "TimeOver")
};
