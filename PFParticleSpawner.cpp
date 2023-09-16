// Fill out your copyright notice in the Description page of Project Settings.

#include "PFParticleSpawner.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

APFParticleSpawner::APFParticleSpawner()
{
	CapsuleComponent->SetCapsuleSize(55.f, 55.0f);
	
	// 파티클 설정
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(CapsuleComponent);
	ParticleSystem->bAutoActivate = false;
	ParticleSystem->SetRelativeScale3D(FVector(8.0f, 8.0f, 8.0f));
	//ParticleSystem'/Game/PF/Effect/Particle/P_Fire.P_Fire'
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleObj(
		TEXT("ParticleSystem'/Game/PF/Effect/Particle/P_Fire.P_Fire'"));
	if (particleObj.Succeeded())
	{
		ParticleSystem->SetTemplate(particleObj.Object);
	}

	spawnDelay = 0.0f;
	maxSpawnDelay = 30.0f;
	canParticleActivate = false;
}

void APFParticleSpawner::BeginPlay()
{
	Super::BeginPlay();

}

void APFParticleSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (canParticleActivate && CanSpawn())
		{
			MulticastActivateParticle();
			canParticleActivate = false;
		}
	}
}

void APFParticleSpawner::ActivateParticle()
{
	// 파티클의 재생 딜레이를 무작위로 설정
	spawnDelay = FMath::RandRange(0.0f, maxSpawnDelay);
	canParticleActivate = true;
}

void APFParticleSpawner::MulticastActivateParticle_Implementation()
{
	// 각 클라이언트에서 파티클 실행
	ParticleSystem->Activate(true);
}