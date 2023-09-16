// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFParticleSpawner.generated.h"

/**
 * 파티클 스포너 클래스
 * 일정 시간의 딜레이 후, 파티클을 재생
 * 붕괴 구역에서 붕괴 이펙트를 나타내기 위해 사용
 */
UCLASS()
class PF_API APFParticleSpawner : public APFSpawner
{
	GENERATED_BODY()

public:
	APFParticleSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* ParticleSystem;
	bool canParticleActivate;

public :
	void ActivateParticle();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastActivateParticle();
};
