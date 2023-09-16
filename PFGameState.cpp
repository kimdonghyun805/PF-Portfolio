// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameState.h"
#include "PFNormalPlayerState.h"
#include "PFBossPlayerState.h"
#include "Net/UnrealNetwork.h"

APFGameState::APFGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsGameStart = false;
	remainGameTime = 0.0f;

	bReplicates = true;
}

void APFGameState::BeginPlay()
{
	Super::BeginPlay();

}

void APFGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		// 게임 진행 시간 계산
		if (remainGameTime > 0.0f)
		{
			remainGameTime -= DeltaTime;
		}
	}
}

void APFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFGameState, bIsGameStart);
	DOREPLIFETIME(APFGameState, remainGameTime);
	DOREPLIFETIME(APFGameState, Players);
	DOREPLIFETIME(APFGameState, NormalPlayerStates);
	DOREPLIFETIME(APFGameState, bossPlayer);
	DOREPLIFETIME(APFGameState, bossPlayerState);
	DOREPLIFETIME(APFGameState, CollapseDistricts);
	DOREPLIFETIME(APFGameState, gameResult);

}

void APFGameState::SetIsGameStart(bool on)
{
	bIsGameStart = on;
}

void APFGameState::SetRemainGameTime(const float& value)
{
	remainGameTime = value;
}

void APFGameState::SetPlayers(TArray<class APlayerController*> players)
{
	Players = players;
	NormalPlayerStates.Empty();
	for (auto& p : Players)
	{
		// 현재 플레이어 중, 일반 캐릭터의 PlayerState를 추출
		if (p->PlayerState != nullptr && p->PlayerState->GetClass()->IsChildOf<APFNormalPlayerState>())
		{
			APFNormalPlayerState* nps = p->GetPlayerState<APFNormalPlayerState>();
			NormalPlayerStates.Emplace(nps);
		}
	}
}

void APFGameState::SetBossPlayer(class APlayerController* boss)
{
	bossPlayer = boss;
	if (bossPlayer != nullptr && bossPlayer->PlayerState->GetClass()->IsChildOf<APFBossPlayerState>())
	{
		bossPlayerState = bossPlayer->GetPlayerState<APFBossPlayerState>();
	}
}

void APFGameState::SetCollapseDistricts(TArray<class APFCollapseDistrict*> districts)
{
	CollapseDistricts = districts;
}

FString APFGameState::GetGameResultMessage(const GameResult& result) const
{
	uint8 index = (uint8)result;
	FString message = FString("None");
	if (index < GameResultMessage.Num())
	{
		message = GameResultMessage[index];
	}
	return message;
}

void APFGameState::SetGameResult(const GameResult& result)
{
	gameResult = result;
}