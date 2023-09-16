// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PFObserverCamera.generated.h"

/**
 * 관전 카메라 클래스
 * 플레이어가 관전 상태일 때 뷰 타겟으로 삼을 카메라 액터
 * 다른 플레이어를 추적하거나, 특정 구역을 관전
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
	class APlayerController* cameraOwner; // 카메라를 뷰 타겟으로 가진 플레이어
	UPROPERTY(Replicated)
	class APlayerController* observingPlayer; // 카메라가 관전 대상으로 삼은 플레이어
	UPROPERTY(Replicated)
	bool bIsFollowing; // 관전 대상 플레이어 추적 여부

public :
	void SetCameraOwner(class APlayerController* controller);
	class APlayerController* GetCameraOwner() const { return cameraOwner; }
	void SetObservingPlayer(class APlayerController* controller);
	class APlayerController* GetObservingPlayer() const { return observingPlayer; }
	void SetIsFollowing(bool on);
	bool GetIsFollowing() const { return bIsFollowing; }
};
