// Fill out your copyright notice in the Description page of Project Settings.

#include "PFSpawner.h"
#include "PF.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

APFSpawner::APFSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &APFSpawner::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APFSpawner::ActorEndOverlap);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetupAttachment(RootComponent);
	CapsuleComponent->SetCapsuleSize(55.f, 96.0f);
	CapsuleComponent->SetCollisionProfileName("OverlapAllDynamics");
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->SetCollisionResponseToChannel(NormalCollisionChannel, ECollisionResponse::ECR_Overlap);
	CapsuleComponent->SetCollisionResponseToChannel(BossCollisionChannel, ECollisionResponse::ECR_Overlap);
	CapsuleComponent->SetCollisionResponseToChannel(ItemCollisionChannel, ECollisionResponse::ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	Mesh->SetupAttachment(CapsuleComponent);
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	bReplicates = true;
}

void APFSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		// 스포너와 겹쳐 있는 일반, 보스 캐릭터, 아이템 액터가 존재하는 경우, 아이템이 스폰되지 않음
		CapsuleComponent->UpdateOverlaps();
		if (spawnDelay > 0.0f && !IsOverlapped())
		{
			spawnDelay -= DeltaTime;
		}
	}
}

void APFSpawner::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OverlappingActors.Find(OtherActor) == INDEX_NONE)
		{
			OverlappingActors.Add(OtherActor);
		}
	}
}

void APFSpawner::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OverlappingActors.Find(OtherActor) != INDEX_NONE)
		{
			OverlappingActors.Remove(OtherActor);
		}
	}
}

bool APFSpawner::CanSpawn() const
{
	if (spawnDelay > 0.0f)
	{
		return false;
	}
	else if (IsOverlapped())
	{
		return false;
	}
	return true;
}

void APFSpawner::SetMaxSpawnDelay(const float& value)
{
	if (value >= 0.0f)
	{
		maxSpawnDelay = value;
	}
}