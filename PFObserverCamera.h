// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PFObserverCamera.generated.h"

/**
 * ���� ī�޶� Ŭ����
 * �÷��̾ ���� ������ �� �� Ÿ������ ���� ī�޶� ����
 * �ٸ� �÷��̾ �����ϰų�, Ư�� ������ ����
 */
UCLASS()
class PF_API APFObserverCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	APFObserverCamera();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected :
	const FVector relativeLocation = FVector(-160.0f, 7.5f, 128.0f);
	UPROPERTY(Replicated)
	FVector attachLocation;
	UPROPERTY(Replicated)
	class APlayerController* cameraOwner; // ī�޶� �� Ÿ������ ���� �÷��̾�
	UPROPERTY(Replicated)
	class APlayerController* observingPlayer; // ī�޶� ���� ������� ���� �÷��̾�
	UPROPERTY(Replicated)
	bool bIsFollowing; // ���� ��� �÷��̾� ���� ����

public :
	void SetCameraOwner(class APlayerController* controller);
	class APlayerController* GetCameraOwner() const { return cameraOwner; }
	void SetObservingPlayer(class APlayerController* controller);
	class APlayerController* GetObservingPlayer() const { return observingPlayer; }
	void SetIsFollowing(bool on);
	bool GetIsFollowing() const { return bIsFollowing; }
};
