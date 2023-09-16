// Fill out your copyright notice in the Description page of Project Settings.


#include "PFNormalPlayerState.h"
#include "PF.h"
#include "Net/UnrealNetwork.h"
#include "PFNormalCharacter.h"
#include "PFGameMode.h"
#include "Camera/CameraActor.h"
#include "PFObserverCamera.h"
#include "PFCollapseDistrict.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"

APFNormalPlayerState::APFNormalPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 10;
	shield = 0.0f;
	life = 3;
	color = FLinearColor::Black;
	canInteraction = false;
	onInteraction = false;
	bAgreeToStart = false;
	
	bReplicates = true;
}

void APFNormalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFNormalPlayerState, health);
	DOREPLIFETIME(APFNormalPlayerState, shield);
	DOREPLIFETIME(APFNormalPlayerState, life);
	DOREPLIFETIME(APFNormalPlayerState, color);
	DOREPLIFETIME(APFNormalPlayerState, normalCharacter);
	DOREPLIFETIME(APFNormalPlayerState, normalController);
	DOREPLIFETIME(APFNormalPlayerState, bIsObserving);
	DOREPLIFETIME(APFNormalPlayerState, observingPlayer);
	DOREPLIFETIME(APFNormalPlayerState, observerCamera);
	DOREPLIFETIME(APFNormalPlayerState, canInteraction);
	DOREPLIFETIME(APFNormalPlayerState, onInteraction);
	DOREPLIFETIME(APFNormalPlayerState, bAgreeToStart);
}

void APFNormalPlayerState::CopyProperties(APlayerState* PlayerState)
{
	// Seamless Travel을 통해 PlayerState를 복제할 때 실행, 변수의 값 등을 복제 가능
	Super::CopyProperties(PlayerState);
	if (PlayerState != nullptr)
	{
		APFNormalPlayerState* normalPS = Cast<APFNormalPlayerState>(PlayerState);
		normalPS->SetColor(color);
		normalPS->SetNormalController(normalController);
	}
}

void APFNormalPlayerState::ResetState()
{
	// 캐릭터 리스폰 시 초기화 되어야 하는 변수
	health = maxHealth;
	shield = 0.0f;
	canInteraction = false;
	onInteraction = false;
}

void APFNormalPlayerState::Observing()
{
	// 사망 시 관전 상태로 전환
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	// 플레이어를 관전 상태로 설정
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->RemovePlayingPlayer(normalController);
	bIsObserving = true;
	gameMode->AddObservingPlayer(normalController);

	// 해당 플레이어를 관전 중인 경우, 대상을 변경
	gameMode->ChangeObservingTargetAboutThisPlayer(normalController);

	// 관전 대상 플레이어를 설정, 카메라 액터를 스폰
	TArray<APlayerController*> targetController = gameMode->GetObservingTargetPlayer();
	if (!(targetController.IsEmpty()))
	{
		int rand = FMath::RandRange(0, (targetController.Num() - 1));
		APFCharacter* targetCharacter = targetController[rand]->GetPawn<APFCharacter>();
		if (targetCharacter != nullptr)
		{
			FVector loc = targetCharacter->GetActorLocation();
			APFObserverCamera* camera = Cast<APFObserverCamera>(GetWorld()->SpawnActor(APFObserverCamera::StaticClass(), &loc));
			camera->SetCameraOwner(normalController);
			camera->SetObservingPlayer(targetController[rand]);
			camera->SetIsFollowing(true);

			SetObserverCamera(camera);
			SetObservingPlayer(targetController[rand]);

			gameMode->AddObserverCamera(camera);

			FViewTargetTransitionParams p;
			p.BlendTime = 1.0f;
			normalController->SetViewTarget(camera, p);
		}
	}
	else // 관전 대상이 없는 경우, 무작위 구역을 관전
	{
		// 관전 대상 구역으로 카메라를 이동
		TArray<APFObserverCamera*> districtCameras = gameMode->GetDistrictObserverCamera();
		int rand = FMath::RandRange(0, (districtCameras.Num() - 1));
		APFObserverCamera* districtCamera = districtCameras[rand];

		FVector loc = normalCharacter->GetActorLocation();
		APFObserverCamera* camera = Cast<APFObserverCamera>(GetWorld()->SpawnActor(APFObserverCamera::StaticClass(), &loc));
		camera->SetCameraOwner(normalController);
		camera->SetObservingPlayer(nullptr);
		camera->SetIsFollowing(false);
		camera->SetActorLocation(districtCamera->GetActorLocation());
		camera->SetActorRotation(districtCamera->GetActorRotation());

		SetObserverCamera(camera);
		SetObservingPlayer(nullptr);

		gameMode->AddObserverCamera(camera);

		FViewTargetTransitionParams p;
		p.BlendTime = 1.0f;
		normalController->SetViewTarget(camera, p);
	}
}

void APFNormalPlayerState::Unobserving()
{
	// 캐릭터 리스폰 시 관전 상태를 해제
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	// 플레이어의 관전 상태 해제
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->AddPlayingPlayer(normalController);
	bIsObserving = false;
	gameMode->RemoveObservingPlayer(normalController);

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

void APFNormalPlayerState::ChangeObservingTarget()
{
	// 관전 대상을 무작위로 변경
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (bIsObserving && observerCamera != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ChangeObservingTarget %s"), *(GetName()));
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

		if (!(targetController.IsEmpty()))
		{
			int rand = FMath::RandRange(0, (targetController.Num() - 1));

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

void APFNormalPlayerState::SetOnInteraction()
{
	if (canInteraction)
	{
		onInteraction = true;
	}
}

void APFNormalPlayerState::SetOffInteraction()
{
	onInteraction = false;
}

void APFNormalPlayerState::SetCanInteraction()
{
	canInteraction = true;
}

void APFNormalPlayerState::SetCanNotInteraction()
{
	canInteraction = false;
}

void APFNormalPlayerState::SetAgreeToStart()
{
	bAgreeToStart = true;
	GetWorld()->GetAuthGameMode<APFGameMode>()->AddAgreeToStartPlayer(normalController);
}

void APFNormalPlayerState::SetDisagreeToStart()
{
	bAgreeToStart = false;
	GetWorld()->GetAuthGameMode<APFGameMode>()->RemoveAgreeToStartPlayer(normalController);
}

void APFNormalPlayerState::SetNormalCharacter(APFNormalCharacter* nc)
{
	normalCharacter = nc;
}

void APFNormalPlayerState::SetNormalController(APlayerController* controller)
{
	normalController = controller;
}

void APFNormalPlayerState::SetObserverCamera(APFObserverCamera* camera)
{
	observerCamera = camera;
}

void APFNormalPlayerState::SetObservingPlayer(APlayerController* controller)
{
	observingPlayer = controller;
}

void APFNormalPlayerState::SetHealth(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	health = value;
	if (health > maxHealth)
	{
		health = maxHealth;
	}
}

void APFNormalPlayerState::DecreaseHealth(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	health -= value;
	if (health < 0.0f)
	{ 
		health = 0.0f; 
	}
}

void APFNormalPlayerState::IncreaseHealth(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	health += value;
	if (health > maxHealth)
	{
		health = maxHealth;
	}
}

void APFNormalPlayerState::SetShield(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}
	
	shield = value;
	if (shield > maxShield)
	{
		shield = maxShield;
	}
}

void APFNormalPlayerState::DecreaseShield(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	shield -= value;
	if (shield < 0.0f)
	{
		shield = 0.0f;
	}
}

void APFNormalPlayerState::IncreaseShield(const float& value)
{
	if (value < 0.0f)
	{
		return;
	}

	shield += value;
	if (shield > maxShield)
	{
		shield = maxShield;
	}
}

void APFNormalPlayerState::SetLife(const uint8& value)
{
	if (value < 0)
	{
		return;
	}

	life = value;
	if (life > maxLife)
	{
		life = maxLife;
	}
}

void APFNormalPlayerState::DecreaseLife(const uint8& value)
{
	if (value < 0)
	{
		return;
	}
	
	life -= value;
	if (life < 0)
	{
		life = 0;
	}
}

void APFNormalPlayerState::IncreaseLife(const uint8& value)
{
	if (value < 0)
	{
		return;
	}

	life += value;
	if (life > maxLife)
	{
		life = maxLife;
	}
}

void APFNormalPlayerState::SetColor(const FLinearColor& c)
{
	color = c;
}