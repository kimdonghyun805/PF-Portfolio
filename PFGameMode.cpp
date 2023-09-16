// Copyright Epic Games, Inc. All Rights Reserved.

#include "PFGameMode.h"
#include "PF.h"
#include "PFGameState.h"
#include "PFCharacter.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "PFBossPlayerState.h"
#include "PFNormalPlayerState.h"
#include "PFNormalHUD.h"
#include "PFBossHUD.h"
#include "PFNormalSpawner.h"
#include "PFBossSpawner.h"
#include "PFWeaponSpawner.h"
#include "PFRecoverySpawner.h"
#include "PFCoreObject.h"
#include "PFCollapseDistrict.h"
#include "PFCenterDistrict.h"
#include "PFObserverCamera.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Engine/World.h"

APFGameMode::APFGameMode()
	: Super()
{
	// 기본 클래스 설정
	HUDClass = APFNormalHUD::StaticClass();
	DefaultPawnClass = APFNormalCharacter::StaticClass();
	PlayerStateClass = APFNormalPlayerState::StaticClass();
	GameStateClass = APFGameState::StaticClass();
	
	// 일반 캐릭터 색상 
	NormalColors.Add(FLinearColor(0.0f, 0.45f, 0.45f, 1.0f)); // green
	NormalColors.Add(FLinearColor(0.7f, 0.7f, 0.0f, 1.0f)); // yellow
	NormalColors.Add(FLinearColor(0.75, 0.0f, 0.0f, 1.0f)); // red
	NormalColors.Add(FLinearColor(0.0f, 0.0f, 0.75f, 1.0f)); // blue
	NormalColors.Add(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f)); // gray
	NormalColors.Add(FLinearColor(0.55f, 0.0f, 0.7f, 1.0f)); // purple
	for (auto& c : NormalColors)
	{
		AvailableNormalColors.Add(c);
	}

	// 변수 초기화
	numNormalCharacter = 0;
	numBossCharacter = 0;
	numDestroyedCore = 0;
	numKilledBoss = 0;
	numKilledNormal = 0;
	gameTime = 0.0f;
	gameResult = GameResult::GameNotEnd;

	bReplicates = true;
	bUseSeamlessTravel = true;
}

void APFGameMode::BeginPlay()
{
	// 서버가 시작되며 실행, 이후에도 Seamless Travel 수행 후에 실행됨
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{ 
		// 맵에 존재하는 오브젝트 확인
		FindCollapseDistrict();
		FindCenterDistrict();
		// 변수 초기화
		numDestroyedCore = 0;
		numKilledBoss = 0;
		numKilledNormal = 0;
		gameTime = 0.0f;
		gameResult = GameResult::GameNotEnd;

		// GameState 변수 설정
		APFGameState* gameState = GetGameState<APFGameState>();
		gameState->SetIsGameStart(bIsGameStart);
		gameState->SetBossPlayer(bossPlayer);
		gameState->SetCollapseDistricts(CollapseDistricts);

		if (bIsGameStart)
		{
			// 게임 맵에서 게임이 시작된 경우, 제한 시간 설정
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(timeOverTimerHandle, this, &APFGameMode::TimeOver, maxGameTime, false);
			gameTime = maxGameTime;
			gameState->SetRemainGameTime(gameTime);
		}
	}
}

void APFGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsGameStart)
	{
		if (gameTime > 0.0f)
		{
			// 게임 시간 계산, 연산에 사용되지 않음
			gameTime -= DeltaTime;
		}

		// 승리 조건 확인
		// 코어를 모두 파괴 -> 보스 승리
		if (numDestroyedCore >= 4) 
		{
			bIsGameStart = false;
			gameResult = GameResult::AllCoreDestroyed;
			GetGameState<APFGameState>()->SetGameResult(gameResult);
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("GameMode - All Core Destroyed - Boss Win"));
		}
		// 모든 일반 캐릭터가 보유한 목숨을 모두 소진 -> 보스 승리
		if (numNormalCharacter > 0 && numKilledNormal >= numNormalCharacter)
		{
			bIsGameStart = false;
			gameResult = GameResult::AllNormalFinished;
			GetGameState<APFGameState>()->SetGameResult(gameResult);
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("GameMode - All Normal Died - Boss Win"));
		}
		// 보스 사망 -> 일반 승리
		if (numBossCharacter > 0 && numKilledBoss >= numBossCharacter)
		{
			bIsGameStart = false;
			gameResult = GameResult::BossFinished;
			GetGameState<APFGameState>()->SetGameResult(gameResult);
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("GameMode - Boss Died - Normal Win"));
		}
		// 게임 시간 종료 -> 일반 승리, TimeOver 함수에서 타이머를 실행
	}
	else
	{	// 참여 중인 모든 플레이어가 시작에 동의한 경우, 게임 시작
		if (AgreeToStartPlayers.Num() > 0 && (Players.Num() == AgreeToStartPlayers.Num()))
		{
			bIsGameStart = true;
			GetGameState<APFGameState>()->SetIsGameStart(bIsGameStart);
			numDestroyedCore = 0;
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameStartTimerHandle, this, &APFGameMode::StartGame, gameStartDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("Game Start"));
		}
	}
}

void APFGameMode::Logout(class AController* Exiting)
{
	Super::Logout(Exiting);
	// 로그아웃한 플레이어 제거
	APlayerController* pc = Cast<APlayerController>(Exiting);
	ChangeObservingTargetAboutThisPlayer(pc);
	RemovePlayer(pc);
	GetGameState<APFGameState>()->SetPlayers(Players);
	RemovePlayingPlayer(pc);
	RemoveObservingPlayer(pc);
	RemoveAgreeToStartPlayer(pc);
	if (pc->PlayerState != nullptr)
	{
		if (pc->PlayerState->GetClass()->IsChildOf<APFNormalPlayerState>())
		{
			APFNormalPlayerState* nps = pc->GetPlayerState<APFNormalPlayerState>();
			FLinearColor color = nps->GetColor();
			AvailableNormalColors.Add(color);
			--numNormalCharacter;
			if (nps->IsObserving())
			{
				APFObserverCamera* camera = nps->GetObserverCamera();
				if (camera != nullptr)
				{
					RemoveObserverCamera(camera);
					camera->Destroy();
				}
			}
		}
		else if (pc->PlayerState->GetClass()->IsChildOf<APFBossPlayerState>())
		{
			APFBossPlayerState* bps = pc->GetPlayerState<APFBossPlayerState>();
			--numBossCharacter;
			bossPlayer = nullptr;
			GetGameState<APFGameState>()->SetBossPlayer(bossPlayer);
			if (bps->IsObserving())
			{
				APFObserverCamera* camera = bps->GetObserverCamera();
				if (camera != nullptr)
				{
					RemoveObserverCamera(camera);
					camera->Destroy();
				}
			}
		}
	}
}

void APFGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	if (bIsGameStart || Players.Num() >= NumMaxPlayer)
	{
		// ErrorMessage 문자열이 비어있지 않은 경우, 로그인 불가능
		ErrorMessage.Append("LoginFailed");
	}
	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

void APFGameMode::PostLogin(APlayerController* NewPlayer)
{
	// 서버에 플레이어가 로그인한 이후 실행
	// 플레이어 정보를 설정한 후, 캐릭터 리스폰
	Super::PostLogin(NewPlayer);
	if (NewPlayer != nullptr)
	{
		APFNormalCharacter* nc = NewPlayer->GetPawn<APFNormalCharacter>();
		if (nc != nullptr)
		{
			nc->Destroy();
		}
		APFNormalPlayerState* nps = NewPlayer->GetPlayerState<APFNormalPlayerState>();
		if (nps != nullptr)
		{
			nps->SetNormalController(NewPlayer);
		}
			
		if (!(AvailableNormalColors.IsEmpty())) // 일반 캐릭터 색상 설정
		{
			int32 rand = FMath::RandRange(0, AvailableNormalColors.Num() - 1);
			FLinearColor color = AvailableNormalColors[rand];
			nps->SetColor(color);
			AvailableNormalColors.Remove(color);
		}

		TSubclassOf<AHUD> normalHUD = APFNormalHUD::StaticClass();
		NewPlayer->ClientSetHUD(normalHUD);

		NormalRespawn(NewPlayer);

		AddPlayer(NewPlayer);
		GetGameState<APFGameState>()->SetPlayers(Players);
		AddPlayingPlayer(NewPlayer);
		++numNormalCharacter;
	}
}

void APFGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
/*
void APFGameMode::PostSeamlessTravel()
{
	UE_LOG(LogTemp, Warning, TEXT("PFGameMode PostSeamlessTravel"));
	Super::PostSeamlessTravel();
}
*/
void APFGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	// Seamless Travel 실행 시 플레이어 캐릭터를 초기화
	// 플레이어의 Controller와 PlayerState 오브젝트는 유지(CopyProperties 함수로 복제됨)
	// Pawn은 제거되어 있음(nullptr)
	Super::InitSeamlessTravelPlayer(NewController);
	APlayerController* pc = Cast<APlayerController>(NewController);

	if (pc != nullptr)
	{
		if (bossPlayer != nullptr && pc == bossPlayer)
		{
			// Pawn 존재하지 않음, PlayerState는 기본 PlayerState로 초기화
			pc->PlayerState->Destroy();
			PlayerStateClass = APFBossPlayerState::StaticClass();
			pc->InitPlayerState();
			APFBossPlayerState* bps = pc->GetPlayerState<APFBossPlayerState>();
			bps->SetBossController(pc);
			bps->SetDisagreeToStart();
			PlayerStateClass = APFNormalPlayerState::StaticClass();
			
			// HUD는 HandleSeamlessTravelPlayer에서 재설정 되어 해당 함수에서는 효과 없음
			//TSubclassOf<AHUD> bossHUD = APFBossHUD::StaticClass(); 
			//pc->ClientSetHUD(bossHUD);

			BossRespawn(pc);
			
			bossPlayer = pc;
			GetGameState<APFGameState>()->SetBossPlayer(pc);
			AddPlayer(pc);
			GetGameState<APFGameState>()->SetPlayers(Players);
			AddPlayingPlayer(pc);
			++numBossCharacter;
		}
		else
		{
			APFNormalPlayerState* nps = pc->GetPlayerState<APFNormalPlayerState>();
			nps->SetNormalController(pc);
			if (AvailableNormalColors.Find(nps->GetColor()) != INDEX_NONE)
			{
				AvailableNormalColors.Remove(nps->GetColor());
			}

			// HUD는 HandleSeamlessTravelPlayer에서 재설정 되어 해당 함수에서는 효과 없음
			//TSubclassOf<AHUD> normalHUD = APFNormalHUD::StaticClass();
			//pc->ClientSetHUD(normalHUD);
			
			NormalRespawn(pc);

			AddPlayer(pc);
			GetGameState<APFGameState>()->SetPlayers(Players);
			AddPlayingPlayer(pc);
			++numNormalCharacter;
		}
	}
}

void APFGameMode::HandleSeamlessTravelPlayer(AController*& NewPlayer)
{
	// 내부에서 InitSeamlessTravelPlayer 함수 실행, 이후 HUD를 설정
	// 따라서, 해당 함수에서 Super를 호출한 후, HUD를 설정해야 캐릭터에 반영
	Super::HandleSeamlessTravelPlayer(NewPlayer);
	APlayerController* pc = Cast<APlayerController>(NewPlayer);
	if (pc->PlayerState->GetClass()->IsChildOf<APFBossPlayerState>())
	{
		TSubclassOf<AHUD> bossHUD = APFBossHUD::StaticClass();
		pc->ClientSetHUD(bossHUD);
	}
	else if (pc->PlayerState->GetClass()->IsChildOf<APFNormalPlayerState>())
	{
		TSubclassOf<AHUD> normalHUD = APFNormalHUD::StaticClass();
		pc->ClientSetHUD(normalHUD);
	}
}
/*
FString APFGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	UE_LOG(LogTemp, Warning, TEXT("InitNewPlayer"));
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}
*/
/*
void APFGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	//UE_LOG(LogTemp, Warning, TEXT("GetSeamlessTravelActorList"));
	for (auto& p : Players)
	{
		ActorList.Emplace(p->PlayerState);
	}
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		APlayerController* pc = Cast<APlayerController>(iter->Get());
		if (pc != nullptr && pc->PlayerState != nullptr)
		{
			ActorList.Emplace(pc->PlayerState);
		}
	}
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
}
*/
void APFGameMode::ChangeNormalToBoss(APlayerController* controller)
{
	// 플레이어 컨트롤러가 조종하는 캐릭터를 일반에서 보스로 변경
	if (numBossCharacter >= NumMaxBossCharacter)
	{
		return;
	}
	ChangeObservingTargetAboutThisPlayer(controller);

	APFNormalCharacter* nc = controller->GetPawn<APFNormalCharacter>();
	nc->DetachFromControllerPendingDestroy();
	nc->Destroy();

	APFNormalPlayerState* nps = controller->GetPlayerState<APFNormalPlayerState>();
	FLinearColor color = nps->GetColor();
	AvailableNormalColors.Add(color);
	controller->PlayerState->Destroy();

	// 기본 PlayerState를 임시로 변경한 후, PlayerState를 초기화
	PlayerStateClass = APFBossPlayerState::StaticClass();
	controller->InitPlayerState();
	APFBossPlayerState* bps = controller->GetPlayerState<APFBossPlayerState>();
	bps->SetBossController(controller);
	bps->SetDisagreeToStart();
	bps->SetReinforceLevel(numDestroyedCore);
	PlayerStateClass = APFNormalPlayerState::StaticClass();

	TSubclassOf<AHUD> bossHUD = APFBossHUD::StaticClass();
	controller->ClientSetHUD(bossHUD);

	BossRespawn(controller);

	--numNormalCharacter;
	++numBossCharacter;
	bossPlayer = controller;
	GetGameState<APFGameState>()->SetBossPlayer(bossPlayer);
	GetGameState<APFGameState>()->SetPlayers(Players);
}

void APFGameMode::ChangeBossToNormal(APlayerController* controller)
{
	// 플레이어 컨트롤러가 조종하는 캐릭터를 보스에서 일반으로 변경
	ChangeObservingTargetAboutThisPlayer(controller);

	APFBossCharacter* bc = controller->GetPawn<APFBossCharacter>();
	bc->DetachFromControllerPendingDestroy();
	bc->Destroy();
	controller->PlayerState->Destroy();

	PlayerStateClass = APFNormalPlayerState::StaticClass();
	controller->InitPlayerState();
	APFNormalPlayerState* nps = Cast<APFNormalPlayerState>(controller->PlayerState);
	nps->SetNormalController(controller);
	nps->SetDisagreeToStart();
	if (!(AvailableNormalColors.IsEmpty()))
	{
		int32 rand = FMath::RandRange(0, AvailableNormalColors.Num() - 1);
		FLinearColor color = AvailableNormalColors[rand];
		nps->SetColor(color);
		AvailableNormalColors.Remove(color);
	}

	TSubclassOf<AHUD> normalHUD = APFNormalHUD::StaticClass();
	controller->ClientSetHUD(normalHUD);

	NormalRespawn(controller);

	--numBossCharacter;
	++numNormalCharacter;
	bossPlayer = nullptr;
	GetGameState<APFGameState>()->SetBossPlayer(bossPlayer);
	GetGameState<APFGameState>()->SetPlayers(Players);
}

void APFGameMode::NormalRespawn(APlayerController* controller)
{
	// 일반 캐릭터를 리스폰, 기존 캐릭터를 제거하고 새로 생성
	if (controller != nullptr)
	{
		FVector loc = DefaultSpawnLocation;
		APFNormalCharacter* newCharacter = Cast<APFNormalCharacter>(GetWorld()->SpawnActor(APFNormalCharacter::StaticClass(), &loc));
		if (newCharacter != nullptr)
		{
			APFNormalPlayerState* nps = controller->GetPlayerState<APFNormalPlayerState>();
			if (nps->IsObserving())
			{
				nps->Unobserving();
			}
			nps->SetNormalCharacter(newCharacter);
			nps->ResetState();
			controller->Possess(newCharacter);
			NormalSpawn(newCharacter);
		}
	}
}

void APFGameMode::BossRespawn(APlayerController* controller)
{
	// 보스 캐릭터를 리스폰, 기존 캐릭터를 제거하고 새로 생성
	if (controller != nullptr)
	{
		FVector loc = DefaultSpawnLocation;
		APFBossCharacter* newCharacter = Cast<APFBossCharacter>(GetWorld()->SpawnActor(APFBossCharacter::StaticClass(), &loc));
		if (newCharacter != nullptr)
		{
			APFBossPlayerState* bps = controller->GetPlayerState<APFBossPlayerState>();
			if (bps->IsObserving())
			{
				bps->Unobserving();
			}
			bps->SetBossCharacter(newCharacter);
			bps->ResetState();
			controller->Possess(newCharacter);
			BossSpawn(newCharacter);
		}
	}
	
}

void APFGameMode::NormalSpawn(APFNormalCharacter* normalCharacter)
{
	// 일반 캐릭터를 스포너 위치에서 생성
	if (GetLocalRole() == ROLE_Authority && normalCharacter != nullptr)
	{
		TArray<APFNormalSpawner*> spawnablePrefer; // 우선 스폰 지점
		TArray<APFNormalSpawner*> spawnable;
		for (auto& district : CollapseDistricts)
		{
			// 붕괴되지 않은 구역을 스폰 지점으로 설정
			if (!(district->IsCollapsed()))
			{
				// 붕괴되지 않은 구역 중에서도 보스가 존재하지 않는 구역을 우선 스폰 지점으로 설정
				if (district->GetOverlappingBosses().IsEmpty())
				{
					for (auto& ns : district->GetNormalSpawners())
					{
						if (ns->CanSpawn())
						{
							spawnablePrefer.Emplace(ns);
							spawnable.Emplace(ns);
						}
					}
				}
				else
				{
					for (auto& ns : district->GetNormalSpawners())
					{
						if (ns->CanSpawn())
						{
							spawnable.Emplace(ns);
						}
					}
				}
				
			}
		}

		if (!(spawnablePrefer.IsEmpty()))
		{
			int32 rand = FMath::RandRange(0, spawnablePrefer.Num() - 1);
			normalCharacter->SetActorLocation(spawnablePrefer[rand]->GetActorLocation());
		}
		else if (!(spawnable.IsEmpty()))
		{
			int32 rand = FMath::RandRange(0, spawnable.Num() - 1);
			normalCharacter->SetActorLocation(spawnable[rand]->GetActorLocation());
		}
		else // 스폰 가능한 지점이 없는 경우, 기본 생성 위치에 스폰
		{
			normalCharacter->SetActorLocation(DefaultSpawnLocation);
		}
		
	}
}

void APFGameMode::BossSpawn(APFBossCharacter* bossCharacter)
{
	// 보스 캐릭터를 스포너 위치에서 생성
	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<APFBossSpawner*> spawnable;
		for (auto& district : CenterDistricts)
		{
			for (auto& bs : district->GetBossSpawners())
			{
				if (bs->CanSpawn())
				{
					spawnable.Emplace(bs);
				}
				
			}
		}

		if (!spawnable.IsEmpty())
		{
			int32 rand = FMath::RandRange(0, spawnable.Num() - 1);
			if (bossCharacter != nullptr)
			{
				bossCharacter->SetActorLocation(spawnable[rand]->GetActorLocation());
			}
		}
		else // 스폰 가능한 지점이 없는 경우, 기본 생성 위치에 스폰
		{
			if (bossCharacter != nullptr)
			{
				bossCharacter->SetActorLocation(DefaultSpawnLocation);
			}
		}
	}
}

void APFGameMode::ChangeObservingTargetAboutThisPlayer(APlayerController* controller)
{
	// 플레이어 컨트롤러의 폰을 관전 중인 다른 플레이어들의 관전 대상을 교체
	if (controller != nullptr)
	{
		for (auto& camera : ObserverCameras)
		{
			APlayerController* pc = camera->GetObservingPlayer(); // pc는 camera의 관전 대상
			if (pc == controller) // 관전 대상이 인자로 지정된 플레이어와 같은 경우
			{
				// camera의 관전 대상을 변경
				APlayerController* playerController = camera->GetCameraOwner();
				if (playerController->PlayerState->GetClass()->IsChildOf<APFNormalPlayerState>())
				{
					APFNormalPlayerState* normalPS = playerController->GetPlayerState<APFNormalPlayerState>();
					normalPS->ChangeObservingTarget();
				}
				else if (playerController->PlayerState->GetClass()->IsChildOf<APFBossPlayerState>())
				{
					APFBossPlayerState* bossPS = playerController->GetPlayerState<APFBossPlayerState>();
					bossPS->ChangeObservingTarget();
				}
			}
		}
	}
}

TArray<APlayerController*> APFGameMode::GetObservingTargetPlayer()
{
	// 관전 대상으로 설정할 수 있는 플레이어 컨트롤러를 탐색
	TArray<APlayerController*> target;
	APlayerController* bossController = nullptr;

	for (auto& p : PlayingPlayers)
	{
		if (p != nullptr)
		{
			APlayerState* ps = p->PlayerState;
			if (ps != nullptr)
			{
				if (ps->GetClass()->IsChildOf<APFNormalPlayerState>())
				{
					target.Emplace(p);
				}
				else if (ps->GetClass()->IsChildOf<APFBossPlayerState>())
				{
					bossController = p;
				}
			}
		}
	}

	if (target.IsEmpty() && bossController != nullptr)
	{
		target.Emplace(bossController);
	}

	return target;
}

TArray<APFObserverCamera*> APFGameMode::GetDistrictObserverCamera()
{
	// 맵의 각 구역의 관전 카메라 탐색
	TArray<APFObserverCamera*> targets;
	for (auto& collpase : CollapseDistricts)
	{
		targets.Emplace(collpase->GetObserverCamera());
	}
	for (auto center : CenterDistricts)
	{
		targets.Emplace(center->GetObserverCamera());
	}

	return targets;
}

void APFGameMode::CoreDestroyed(APFCoreObject* core)
{
	// 구역의 코어가 파괴된 경우 실행
	++numDestroyedCore;
	if (bossPlayer != nullptr)
	{
		APFBossPlayerState* bossPS = bossPlayer->GetPlayerState<APFBossPlayerState>();
		bossPS->IncreaseReinforceLevel();
		bossPS->IncreaseHealth(bossPS->GetMaxHealth() / 10.0f);
	}
	
	for (auto& district : CollapseDistricts)
	{
		if (!(district->IsCollapsed()))
		{
			for (auto& ws : district->GetWeaponSpawners())
			{
				ws->IncreaseReinforceLevel();
			}
			for (auto& rs : district->GetRecoverySpawners())
			{
				rs->IncreaseReinforceLevel();
			}
		}
	}
}

void APFGameMode::BossFinished(APFBossPlayerState* bossPS)
{
	// 게임 진행 중, 보스가 모든 체력을 소진했을 때 실행
	if (bossPS->GetHealth() <= 0.0f)
	{
		++numKilledBoss;
	}
}

void APFGameMode::NormalFinished(APFNormalPlayerState* normalPS)
{
	// 게임 진행 중, 일반 캐릭터가 모든 목숨과 체력을 소진했을 때 실행
	if (normalPS->GetLife() <= 0)
	{
		++numKilledNormal;
	}
}

void APFGameMode::FindCollapseDistrict()
{
	// 맵에 존재하는 PFCollapseDistrict 탐색
	for (TActorIterator<APFCollapseDistrict> iter(GetWorld()); iter; ++iter)
	{
		CollapseDistricts.Emplace(*iter);
	}
}

void APFGameMode::FindCenterDistrict()
{
	// 맵에 존재하는 PFCenterDistrict 탐색
	for (TActorIterator<APFCenterDistrict> iter(GetWorld()); iter; ++iter)
	{
		CenterDistricts.Emplace(*iter);
	}
}

void APFGameMode::StartGame()
{
	// 게임 실행
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(gameStartTimerHandle);

	numDestroyedCore = 0;

	// 보스 캐릭터가 존재하지 않는 경우, 무작위 플레이어를 보스로 전환
	if (bossPlayer == nullptr)
	{
		int32 rand = FMath::RandRange(0, Players.Num() - 1);
		bossPlayer = Players[rand];
		GetGameState<APFGameState>()->SetBossPlayer(bossPlayer);
	}

	// 게임 맵으로 이동
	//World'/Game/PF/Level/GameLevel.GameLevel'
	// L"/Game/PF/Level/GameLevel?Listen"
	GetWorld()->ServerTravel(L"/Game/PF/Level/GameLevel?Listen");
}

void APFGameMode::EndGame()
{
	// 게임 종료
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(gameEndTimerHandle);
	worldTimerManager.ClearTimer(timeOverTimerHandle);
	
	bIsGameStart = false;
	GetGameState<APFGameState>()->SetIsGameStart(bIsGameStart);
	numDestroyedCore = 0;

	// 대기 맵으로 이동
	//World'/Game/PF/Level/WaitingLevel.WaitingLevel'
	// L"/Game/PF/Level/WaitingLevel?Listen"
	GetWorld()->ServerTravel(L"/Game/PF/Level/WaitingLevel?Listen");
}

void APFGameMode::TimeOver()
{
	// 게임 제한 시간 종료
	gameResult = GameResult::TimeOver;
	GetGameState<APFGameState>()->SetGameResult(gameResult);
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	//worldTimerManager.ClearTimer(timeOverTimerHandle); // EndGame에서 타이머 핸들을 제거하므로, 해당 함수에서 실행할 필요 없음

	// 게임 종료
	worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
}

void APFGameMode::AddPlayer(APlayerController* controller)
{
	if (Players.Find(controller) == INDEX_NONE)
	{
		Players.Emplace(controller);
	}
}
void APFGameMode::RemovePlayer(APlayerController* controller)
{
	if (Players.Find(controller) != INDEX_NONE)
	{
		Players.Remove(controller);
	}
}
void APFGameMode::AddObservingPlayer(APlayerController* controller)
{
	if (ObservingPlayers.Find(controller) == INDEX_NONE)
	{
		ObservingPlayers.Emplace(controller);
	}
}
void APFGameMode::RemoveObservingPlayer(APlayerController* controller)
{
	if (ObservingPlayers.Find(controller) != INDEX_NONE)
	{
		ObservingPlayers.Remove(controller);
	}
}
void APFGameMode::AddPlayingPlayer(APlayerController* controller)
{
	if (PlayingPlayers.Find(controller) == INDEX_NONE)
	{
		PlayingPlayers.Emplace(controller);
	}
}
void APFGameMode::RemovePlayingPlayer(APlayerController* controller)
{
	if (PlayingPlayers.Find(controller) != INDEX_NONE)
	{
		PlayingPlayers.Remove(controller);
	}
}
void APFGameMode::AddAgreeToStartPlayer(APlayerController* controller)
{
	if (AgreeToStartPlayers.Find(controller) == INDEX_NONE)
	{
		AgreeToStartPlayers.Emplace(controller);
	}
}
void APFGameMode::RemoveAgreeToStartPlayer(APlayerController* controller)
{
	if (AgreeToStartPlayers.Find(controller) != INDEX_NONE)
	{
		AgreeToStartPlayers.Remove(controller);
	}
}
void APFGameMode::AddObserverCamera(APFObserverCamera* camera)
{
	if (ObserverCameras.Find(camera) == INDEX_NONE)
	{
		ObserverCameras.Emplace(camera);
	}
}
void APFGameMode::RemoveObserverCamera(APFObserverCamera* camera)
{
	if (ObserverCameras.Find(camera) != INDEX_NONE)
	{
		ObserverCameras.Remove(camera);
	}
}
