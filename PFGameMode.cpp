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
	// �⺻ Ŭ���� ����
	HUDClass = APFNormalHUD::StaticClass();
	DefaultPawnClass = APFNormalCharacter::StaticClass();
	PlayerStateClass = APFNormalPlayerState::StaticClass();
	GameStateClass = APFGameState::StaticClass();
	
	// �Ϲ� ĳ���� ���� 
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

	// ���� �ʱ�ȭ
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
	// ������ ���۵Ǹ� ����, ���Ŀ��� Seamless Travel ���� �Ŀ� �����
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{ 
		// �ʿ� �����ϴ� ������Ʈ Ȯ��
		FindCollapseDistrict();
		FindCenterDistrict();
		// ���� �ʱ�ȭ
		numDestroyedCore = 0;
		numKilledBoss = 0;
		numKilledNormal = 0;
		gameTime = 0.0f;
		gameResult = GameResult::GameNotEnd;

		// GameState ���� ����
		APFGameState* gameState = GetGameState<APFGameState>();
		gameState->SetIsGameStart(bIsGameStart);
		gameState->SetBossPlayer(bossPlayer);
		gameState->SetCollapseDistricts(CollapseDistricts);

		if (bIsGameStart)
		{
			// ���� �ʿ��� ������ ���۵� ���, ���� �ð� ����
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
			// ���� �ð� ���, ���꿡 ������ ����
			gameTime -= DeltaTime;
		}

		// �¸� ���� Ȯ��
		// �ھ ��� �ı� -> ���� �¸�
		if (numDestroyedCore >= 4) 
		{
			bIsGameStart = false;
			gameResult = GameResult::AllCoreDestroyed;
			GetGameState<APFGameState>()->SetGameResult(gameResult);
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("GameMode - All Core Destroyed - Boss Win"));
		}
		// ��� �Ϲ� ĳ���Ͱ� ������ ����� ��� ���� -> ���� �¸�
		if (numNormalCharacter > 0 && numKilledNormal >= numNormalCharacter)
		{
			bIsGameStart = false;
			gameResult = GameResult::AllNormalFinished;
			GetGameState<APFGameState>()->SetGameResult(gameResult);
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("GameMode - All Normal Died - Boss Win"));
		}
		// ���� ��� -> �Ϲ� �¸�
		if (numBossCharacter > 0 && numKilledBoss >= numBossCharacter)
		{
			bIsGameStart = false;
			gameResult = GameResult::BossFinished;
			GetGameState<APFGameState>()->SetGameResult(gameResult);
			FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
			worldTimerManager.SetTimer(gameEndTimerHandle, this, &APFGameMode::EndGame, gameEndDelay, false);
			//UE_LOG(LogTemp, Warning, TEXT("GameMode - Boss Died - Normal Win"));
		}
		// ���� �ð� ���� -> �Ϲ� �¸�, TimeOver �Լ����� Ÿ�̸Ӹ� ����
	}
	else
	{	// ���� ���� ��� �÷��̾ ���ۿ� ������ ���, ���� ����
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
	// �α׾ƿ��� �÷��̾� ����
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
		// ErrorMessage ���ڿ��� ������� ���� ���, �α��� �Ұ���
		ErrorMessage.Append("LoginFailed");
	}
	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

void APFGameMode::PostLogin(APlayerController* NewPlayer)
{
	// ������ �÷��̾ �α����� ���� ����
	// �÷��̾� ������ ������ ��, ĳ���� ������
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
			
		if (!(AvailableNormalColors.IsEmpty())) // �Ϲ� ĳ���� ���� ����
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
	// Seamless Travel ���� �� �÷��̾� ĳ���͸� �ʱ�ȭ
	// �÷��̾��� Controller�� PlayerState ������Ʈ�� ����(CopyProperties �Լ��� ������)
	// Pawn�� ���ŵǾ� ����(nullptr)
	Super::InitSeamlessTravelPlayer(NewController);
	APlayerController* pc = Cast<APlayerController>(NewController);

	if (pc != nullptr)
	{
		if (bossPlayer != nullptr && pc == bossPlayer)
		{
			// Pawn �������� ����, PlayerState�� �⺻ PlayerState�� �ʱ�ȭ
			pc->PlayerState->Destroy();
			PlayerStateClass = APFBossPlayerState::StaticClass();
			pc->InitPlayerState();
			APFBossPlayerState* bps = pc->GetPlayerState<APFBossPlayerState>();
			bps->SetBossController(pc);
			bps->SetDisagreeToStart();
			PlayerStateClass = APFNormalPlayerState::StaticClass();
			
			// HUD�� HandleSeamlessTravelPlayer���� �缳�� �Ǿ� �ش� �Լ������� ȿ�� ����
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

			// HUD�� HandleSeamlessTravelPlayer���� �缳�� �Ǿ� �ش� �Լ������� ȿ�� ����
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
	// ���ο��� InitSeamlessTravelPlayer �Լ� ����, ���� HUD�� ����
	// ����, �ش� �Լ����� Super�� ȣ���� ��, HUD�� �����ؾ� ĳ���Ϳ� �ݿ�
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
	// �÷��̾� ��Ʈ�ѷ��� �����ϴ� ĳ���͸� �Ϲݿ��� ������ ����
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

	// �⺻ PlayerState�� �ӽ÷� ������ ��, PlayerState�� �ʱ�ȭ
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
	// �÷��̾� ��Ʈ�ѷ��� �����ϴ� ĳ���͸� �������� �Ϲ����� ����
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
	// �Ϲ� ĳ���͸� ������, ���� ĳ���͸� �����ϰ� ���� ����
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
	// ���� ĳ���͸� ������, ���� ĳ���͸� �����ϰ� ���� ����
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
	// �Ϲ� ĳ���͸� ������ ��ġ���� ����
	if (GetLocalRole() == ROLE_Authority && normalCharacter != nullptr)
	{
		TArray<APFNormalSpawner*> spawnablePrefer; // �켱 ���� ����
		TArray<APFNormalSpawner*> spawnable;
		for (auto& district : CollapseDistricts)
		{
			// �ر����� ���� ������ ���� �������� ����
			if (!(district->IsCollapsed()))
			{
				// �ر����� ���� ���� �߿����� ������ �������� �ʴ� ������ �켱 ���� �������� ����
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
		else // ���� ������ ������ ���� ���, �⺻ ���� ��ġ�� ����
		{
			normalCharacter->SetActorLocation(DefaultSpawnLocation);
		}
		
	}
}

void APFGameMode::BossSpawn(APFBossCharacter* bossCharacter)
{
	// ���� ĳ���͸� ������ ��ġ���� ����
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
		else // ���� ������ ������ ���� ���, �⺻ ���� ��ġ�� ����
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
	// �÷��̾� ��Ʈ�ѷ��� ���� ���� ���� �ٸ� �÷��̾���� ���� ����� ��ü
	if (controller != nullptr)
	{
		for (auto& camera : ObserverCameras)
		{
			APlayerController* pc = camera->GetObservingPlayer(); // pc�� camera�� ���� ���
			if (pc == controller) // ���� ����� ���ڷ� ������ �÷��̾�� ���� ���
			{
				// camera�� ���� ����� ����
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
	// ���� ������� ������ �� �ִ� �÷��̾� ��Ʈ�ѷ��� Ž��
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
	// ���� �� ������ ���� ī�޶� Ž��
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
	// ������ �ھ �ı��� ��� ����
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
	// ���� ���� ��, ������ ��� ü���� �������� �� ����
	if (bossPS->GetHealth() <= 0.0f)
	{
		++numKilledBoss;
	}
}

void APFGameMode::NormalFinished(APFNormalPlayerState* normalPS)
{
	// ���� ���� ��, �Ϲ� ĳ���Ͱ� ��� ����� ü���� �������� �� ����
	if (normalPS->GetLife() <= 0)
	{
		++numKilledNormal;
	}
}

void APFGameMode::FindCollapseDistrict()
{
	// �ʿ� �����ϴ� PFCollapseDistrict Ž��
	for (TActorIterator<APFCollapseDistrict> iter(GetWorld()); iter; ++iter)
	{
		CollapseDistricts.Emplace(*iter);
	}
}

void APFGameMode::FindCenterDistrict()
{
	// �ʿ� �����ϴ� PFCenterDistrict Ž��
	for (TActorIterator<APFCenterDistrict> iter(GetWorld()); iter; ++iter)
	{
		CenterDistricts.Emplace(*iter);
	}
}

void APFGameMode::StartGame()
{
	// ���� ����
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(gameStartTimerHandle);

	numDestroyedCore = 0;

	// ���� ĳ���Ͱ� �������� �ʴ� ���, ������ �÷��̾ ������ ��ȯ
	if (bossPlayer == nullptr)
	{
		int32 rand = FMath::RandRange(0, Players.Num() - 1);
		bossPlayer = Players[rand];
		GetGameState<APFGameState>()->SetBossPlayer(bossPlayer);
	}

	// ���� ������ �̵�
	//World'/Game/PF/Level/GameLevel.GameLevel'
	// L"/Game/PF/Level/GameLevel?Listen"
	GetWorld()->ServerTravel(L"/Game/PF/Level/GameLevel?Listen");
}

void APFGameMode::EndGame()
{
	// ���� ����
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(gameEndTimerHandle);
	worldTimerManager.ClearTimer(timeOverTimerHandle);
	
	bIsGameStart = false;
	GetGameState<APFGameState>()->SetIsGameStart(bIsGameStart);
	numDestroyedCore = 0;

	// ��� ������ �̵�
	//World'/Game/PF/Level/WaitingLevel.WaitingLevel'
	// L"/Game/PF/Level/WaitingLevel?Listen"
	GetWorld()->ServerTravel(L"/Game/PF/Level/WaitingLevel?Listen");
}

void APFGameMode::TimeOver()
{
	// ���� ���� �ð� ����
	gameResult = GameResult::TimeOver;
	GetGameState<APFGameState>()->SetGameResult(gameResult);
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	//worldTimerManager.ClearTimer(timeOverTimerHandle); // EndGame���� Ÿ�̸� �ڵ��� �����ϹǷ�, �ش� �Լ����� ������ �ʿ� ����

	// ���� ����
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
