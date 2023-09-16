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
	// Seamless Travel�� ���� PlayerState�� ������ �� ����, ������ �� ���� ���� ����
	Super::CopyProperties(PlayerState);
	if (PlayerState != nullptr)
	{
		// PlayerState�� Seamless�� ����Ǵ� ���, �⺻ PlayerState Ŭ������ �����
		// BossPlayerState�� Copy�� ���������, ���� �� ������Ʈ�� ������ PlayerState�� NormalPlayerState�̹Ƿ� ���������� ������� ����
		//UE_LOG(LogTemp, Warning, TEXT("BossPlayerState Copy %s"), *(PlayerState->GetName()));
	}
}

void APFBossPlayerState::ResetState()
{
	// ĳ���� ������ �� �ʱ�ȭ �Ǿ�� �ϴ� ����
	canInteraction = false;
	onInteraction = false;
}

void APFBossPlayerState::Observing()
{
	// ĳ���� ��� �� ���� ���·� ��ȯ
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	// �÷��̾ ���� ���·� ����
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->RemovePlayingPlayer(bossController);
	bIsObserving = true;
	gameMode->AddObservingPlayer(bossController);

	// �ش� �÷��̾ ���� ���� �ٸ� �÷��̾��� ���� ����� ����
	gameMode->ChangeObservingTargetAboutThisPlayer(bossController);

	// ���� ��� �÷��̾ �����ϰ�, ī�޶� ���͸� ����
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
	else // ���� ����� ���� ���, ������ ������ ����
	{
		// ���� ��� �������� ī�޶� �̵�
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
	// ĳ���� ������ �� ���� ���¸� ����
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	// �÷��̾��� ���� ���� ����
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->AddPlayingPlayer(bossController);
	bIsObserving = false;
	gameMode->RemoveObservingPlayer(bossController);

	// ī�޶� ���͸� ����
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
	// ���� ����� �������� ����
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

		if (!(targetController.IsEmpty())) // ���� ��� �÷��̾ ����
		{
			int rand = FMath::RandRange(0, targetController.Num() - 1);

			observerCamera->SetObservingPlayer(targetController[rand]);
			observerCamera->SetIsFollowing(true);

			SetObservingPlayer(targetController[rand]);
		}
		else // ���� ����� ���� ���, ������ ������ ����
		{
			// ���� ��� �������� ī�޶� �̵�
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