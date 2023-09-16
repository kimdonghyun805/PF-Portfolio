// Fill out your copyright notice in the Description page of Project Settings.


#include "PFObserverCamera.h"
#include "PFCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"

APFObserverCamera::APFObserverCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	SetActorRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));

	GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Perspective);
	GetCameraComponent()->SetFieldOfView(90.0f);
	GetCameraComponent()->SetConstraintAspectRatio(false);

	DisableComponentsSimulatePhysics();
	cameraOwner = nullptr;
	observingPlayer = nullptr;
	bIsFollowing = true;

	bReplicates = true;
}

void APFObserverCamera::BeginPlay()
{
	Super::BeginPlay();

}

void APFObserverCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		// ���� ��� �÷��̾ �����ϸ�, ī�޶� ����� �̵��� ����
		if (bIsFollowing && observingPlayer != nullptr)
		{
			SetActorRelativeLocation(attachLocation);
		}
	}
}

void APFObserverCamera::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFObserverCamera, attachLocation);
	DOREPLIFETIME(APFObserverCamera, cameraOwner);
	DOREPLIFETIME(APFObserverCamera, observingPlayer);
	DOREPLIFETIME(APFObserverCamera, bIsFollowing);
}

void APFObserverCamera::SetCameraOwner(APlayerController* controller)
{
	cameraOwner = controller;
}

void APFObserverCamera::SetObservingPlayer(APlayerController* controller)
{
	// ī�޶��� ���� ��� �÷��̾ ����
	observingPlayer = controller;
	if (controller != nullptr)
	{
		AttachToActor(controller->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
		if (controller->GetPawn() != nullptr)
		{
			// ĳ������ ������ ���� ī�޶��� ��ġ�� ����
			if (controller->GetPawn()->GetClass()->IsChildOf<APFBossCharacter>())
			{
				attachLocation = 2 * relativeLocation;
			}
			else if (controller->GetPawn()->GetClass()->IsChildOf<APFNormalCharacter>())
			{
				attachLocation = relativeLocation;
			}
		}
		SetActorRelativeLocation(attachLocation);
	}
}

void APFObserverCamera::SetIsFollowing(bool on)
{
	bIsFollowing = on;
}