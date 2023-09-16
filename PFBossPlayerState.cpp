// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBossPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PFBossCharacter.h"
#include "PFGameMode.h"
#include "PFObserverCamera.h"

APFBossPlayerState::APFBossPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = maxHealth;
	reinforceLevel = 0;
	canInteraction = false;
	onInteraction = false;
	bAgreeToStart = false;

	bReplicates = true;
}

void APFBossPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFBossPlayerState, health);
	DOREPLIFETIME(APFBossPlayerState, reinforceLevel);
	DOREPLIFETIME(APFBossPlayerState, bossCharacter);
	DOREPLIFETIME(APFBossPlayerState, bossController);
	DOREPLIFETIME(APFBossPlayerState, bIsObserving);
	DOREPLIFETIME(APFBossPlayerState, observingPlayer);
	DOREPLIFETIME(APFBossPlayerState, observerCamera);
	DOREPLIFETIME(APFBossPlayerState, canInteraction);
	DOREPLIFETIME(APFBossPlayerState, onInteraction);
	DOREPLIFETIME(APFBossPlayerState, bAgreeToStart);
}

void APFBossPlayerState::CopyProperties(APlayerState* PlayerState)
{
	// Seamless Travel을 통해 PlayerState를 복제할 때 실행, 변수의 값 등을 복제 가능
	Super::CopyProperties(PlayerState);
	if (PlayerState != nullptr)
	{
		// PlayerState가 Seamless로 복사되는 경우, 기본 PlayerState 클래스로 복사됨
		// BossPlayerState의 Copy가 실행되지만, 복사 될 오브젝트인 인자의 PlayerState는 NormalPlayerState이므로 정상적으로 실행되지 않음
		//UE_LOG(LogTemp, Warning, TEXT("BossPlayerState Copy %s"), *(PlayerState->GetName()));
	}
}

void APFBossPlayerState::ResetState()
{
	// 캐릭터 리스폰 시 초기화 되어야 하는 변수
	canInteraction = false;
	onInteraction = false;
}

void APFBossPlayerState::Observing()
{
	// 캐릭터 사망 시 관전 상태로 전환
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	// 플레이어를 관전 상태로 설정
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->RemovePlayingPlayer(bossController);
	bIsObserving = true;
	gameMode->AddObservingPlayer(bossController);

	// 해당 플레이어를 관전 중인 다른 플레이어의 관전 대상을 변경
	gameMode->ChangeObservingTargetAboutThisPlayer(bossController);

	// 관전 대상 플레이어를 설정하고, 카메라 액터를 스폰
	TArray<APlayerController*> targetController = gameMode->GetObservingTargetPlayer();
	if (!(targetController.IsEmpty()))
	{
		int rand = FMath::RandRange(0, (targetController.Num() - 1));
		APFCharacter* targetCharacter = targetController[rand]->GetPawn<APFCharacter>();
		if (targetCharacter != nullptr)
		{
			FVector loc = targetCharacter->GetActorLocation();
			APFObserverCamera* camera = Cast<APFObserverCamera>(GetWorld()->SpawnActor(APFObserverCamera::StaticClass(), &loc));
			camera->SetCameraOwner(bossController);
			camera->SetObservingPlayer(targetController[rand]);
			camera->SetIsFollowing(true);

			SetObserverCamera(camera);
			SetObservingPlayer(targetController[rand]);

			gameMode->AddObserverCamera(camera);

			FViewTargetTransitionParams p;
			p.BlendTime = 1.0f;
			bossController->SetViewTarget(camera, p);
		}
	}
	else // 관전 대상이 없는 경우, 무작위 구역을 관전
	{
		// 관전 대상 구역으로 카메라를 이동
		TArray<APFObserverCamera*> districtCameras = gameMode->GetDistrictObserverCamera();
		int rand = FMath::RandRange(0, (districtCameras.Num() - 1));
		APFObserverCamera* districtCamera = districtCameras[rand];

		FVector loc = bossCharacter->GetActorLocation();
		APFObserverCamera* camera = Cast<APFObserverCamera>(GetWorld()->SpawnActor(APFObserverCamera::StaticClass(), &loc));
		camera->SetCameraOwner(bossController);
		camera->SetObservingPlayer(nullptr);
		camera->SetIsFollowing(false);
		camera->SetActorLocation(districtCamera->GetActorLocation());
		camera->SetActorRotation(districtCamera->GetActorRotation());

		SetObserverCamera(camera);
		SetObservingPlayer(nullptr);

		gameMode->AddObserverCamera(camera);

		FViewTargetTransitionParams p;
		p.BlendTime = 1.0f;
		bossController->SetViewTarget(camera, p);
	}
}

void APFBossPlayerState::Unobserving()
{
	// 캐릭터 리스폰 시 관전 상태를 해제
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	// 플레이어의 관전 상태 해제
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->AddPlayingPlayer(bossController);
	bIsObserving = false;
	gameMode->RemoveObservingPlayer(bossController);

	// 카메라 액터를 제거
	if (observerCamera != nullptr)
	{
		gameMode->RemoveObserverCamera(observerCamera);
		observerCamera->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		observerCamera->Destroy();
	}
	observerCamera = nullptr;
	observingPlayer = nullptr;
}

void APFBossPlayerState::ChangeObservingTarget()
{
	// 관전 대상을 무작위로 변경
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (bIsObserving && observerCamera != nullptr)
	{
		APFGameMode* gameMode = GetWorld()->GetAuthGameMode<APFGameMode>();
		TArray<APlayerController*> targetController = gameMode->GetObservingTargetPlayer();
		APlayerController* selfController = nullptr;
		for (auto& t : targetController)
		{
			if (observingPlayer != nullptr && t == observingPlayer)
			{
				selfController = t;
			}
		}
		if (selfController != nullptr)
		{
			targetController.Remove(selfController);
		}

		if (!(targetController.IsEmpty())) // 관전 대상 플레이어가 존재
		{
			int rand = FMath::RandRange(0, targetController.Num() - 1);

			observerCamera->SetObservingPlayer(targetController[rand]);
			observerCamera->SetIsFollowing(true);

			SetObservingPlayer(targetController[rand]);
		}
		else // 관전 대상이 없는 경우, 무작위 구역을 관전
		{
			// 관전 대상 구역으로 카메라를 이동
			TArray<APFObserverCamera*> districtCameras = gameMode->GetDistrictObserverCamera();
			int rand = FMath::RandRange(0, (districtCameras.Num() - 1));
			APFObserverCamera* districtCamera = districtCameras[rand];

			observerCamera->AttachToActor(districtCamera, FAttachmentTransformRules::KeepRelativeTransform);
			observerCamera->SetObservingPlayer(nullptr);
			observerCamera->SetIsFollowing(false);
			observerCamera->SetActorLocation(districtCamera->GetActorLocation());
			observerCamera->SetActorRotation(districtCamera->GetActorRotation());

			SetObservingPlayer(nullptr);
		}
	}
}

void APFBossPlayerState::SetOnInteraction()
{
	if (canInteraction)
	{
		onInteraction = true;
	}
}

void APFBossPlayerState::SetOffInteraction()
{
	onInteraction = false;
}

void APFBossPlayerState::SetCanInteraction()
{
	canInteraction = true;
}

void APFBossPlayerState::SetCanNotInteraction()
{
	canInteraction = false;
}

void APFBossPlayerState::SetAgreeToStart()
{
	bAgreeToStart = true;
	GetWorld()->GetAuthGameMode<APFGameMode>()->AddAgreeToStartPlayer(bossController);
}

void APFBossPlayerState::SetDisagreeToStart()
{
	bAgreeToStart = false;
	GetWorld()->GetAuthGameMode<APFGameMode>()->RemoveAgreeToStartPlayer(bossController);
}

void APFBossPlayerState::SetBossCharacter(APFBossCharacter* bc)
{
	bossCharacter = bc;
}

void APFBossPlayerState::SetBossController(APlayerController* controller)
{
	bossController = controller;
}

void APFBossPlayerState::SetObserverCamera(APFObserverCamera* camera)
{
	observerCamera = camera;
}

void APFBossPlayerState::SetObservingPlayer(APlayerController* controller)
{
	observingPlayer = controller;
}

void APFBossPlayerState::SetReinforceLevel(const uint8& value)
{
	reinforceLevel = value;
	if (bossCharacter != nullptr)
	{
		bossCharacter->Reinforce(reinforceLevel);
	}
}

void APFBossPlayerState::IncreaseReinforceLevel()
{
	++reinforceLevel;
	if (bossCharacter != nullptr)
	{
		bossCharacter->Reinforce(reinforceLevel);
	}
}

void APFBossPlayerState::DecreaseReinforceLevel()
{
	--reinforceLevel;
	if (bossCharacter != nullptr)
	{
		bossCharacter->Reinforce(reinforceLevel);
	}
}

void APFBossPlayerState::SetHealth(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	if (health + value > maxHealth)
	{
		health = maxHealth;
	}
	else
	{
		health = value;
	}
}

void APFBossPlayerState::IncreaseHealth(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	if (health + value > maxHealth)
	{
		health = maxHealth;
	}
	else
	{
		health += value;
	}
}

void APFBossPlayerState::DecreaseHealth(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	health -= value;
}