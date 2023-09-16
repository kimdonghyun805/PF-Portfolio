// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFParticleSpawner.generated.h"

/**
 * ��ƼŬ ������ Ŭ����
 * ���� �ð��� ������ ��, ��ƼŬ�� ���
 * �ر� �������� �ر� ����Ʈ�� ��Ÿ���� ���� ���
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
