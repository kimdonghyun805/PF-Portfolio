// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameMode.h"
#include "GameFramework/GameState.h"
#include "PFGameState.generated.h"

/**
 * 게임 스테이트 클래스
 * 게임의 진행 상태 정보를 게임 모드로 부터 전달받아 보유
 */
UCLASS()
class PF_API APFGameState : public AGameState
{
	GENERATED_BODY()

public :
	APFGameState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected :
	// 게임 진행 상황을 나타내는 변수 설정
	UPROPERTY(Replicated)
	bool bIsGameStart;
	UPROPERTY(Replicated)
	float remainGameTime;
	UPROPERTY(Replicated)
	TArray<class APlayerController*> Players;
	UPROPERTY(Replicated)
	TArray<class APFNormalPlayerState*> NormalPlayerStates;
	UPROPERTY(Replicated)
	class APlayerController* bossPlayer;
	UPROPERTY(Replicated)
	class APFBossPlayerState* bossPlayerState;
	UPROPERTY(Replicated)
	TArray<class APFCollapseDistrict*> CollapseDistricts;
	UPROPERTY(Replicated)
	GameResult gameResult;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> GameResultMessage = {
	FString(""),
	FString("All Core Objests are Destroyed!\nBoss Player Win!"),
	FString("All Normal Characters are Dead\nBoss Player Win!"),
	FString("BOSS Character is Dead\nNormal Players Win!"),
	FString("Time is Over!\nNormal Players Win!")
	};

public:
	void SetIsGameStart(bool on);
	void SetRemainGameTime(const float& value);
	void SetPlayers(TArray<class APlayerController*> players);
	void SetBossPlayer(class APlayerController* boss);
	void SetCollapseDistricts(TArray<class APFCollapseDistrict*> districts);
	void SetGameResult(const GameResult& result);

	UFUNCTION(BlueprintCallable)
	bool IsGameStart() const { return bIsGameStart; }
	UFUNCTION(BlueprintCallable)
	float GetRemainGameTime() const { return remainGameTime; }
	UFUNCTION(BlueprintCallable)
	TArray<class APlayerController*> GetPlayers() const { return Players; }
	UFUNCTION(BlueprintCallable)
	TArray<class APFNormalPlayerState*> GetNormalPlayerStates() const { return NormalPlayerStates; }
	UFUNCTION(BlueprintCallable)
	class APlayerController* GetBossPlayer() const { return bossPlayer; }
	UFUNCTION(BlueprintCallable)
	class APFBossPlayerState* GetBossPlayerState() const { return bossPlayerState; }
	UFUNCTION(BlueprintCallable)
	TArray<class APFCollapseDistrict*> GetCollapseDistricts() const { return CollapseDistricts; }
	UFUNCTION(BlueprintCallable)
	GameResult GetGameResult() const { return gameResult; }
	UFUNCTION(BlueprintCallable)
	FString GetGameResultMessage(const GameResult& result) const;
	
};
