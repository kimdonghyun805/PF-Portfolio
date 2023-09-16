// Fill out your copyright notice in the Description page of Project Settings.


#include "PFDistrict.h"
#include "PF.h"
#include "PFNormalCharacter.h"
#include "PFBossCharacter.h"
#include "PFObserverCamera.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APFDistrict::APFDistrict()
{
	PrimaryActorTick.bCanEverTick = true;
	OnActorBeginOverlap.AddDynamic(this, &APFDistrict::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APFDistrict::ActorEndOverlap);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));
	BoxComponent->SetCollisionProfileName("OverlapAllDynamics");
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionResponseToChannel(NormalCollisionChannel, ECollisionResponse::ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(BossCollisionChannel, ECollisionResponse::ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ItemCollisionChannel, ECollisionResponse::ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DistrictMesh"));
	Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Mesh->SetIsReplicated(true);
	Mesh->SetupAttachment(BoxComponent);
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	bReplicates = true;
}

void APFDistrict::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFDistrict::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APFDistrict::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// 구역에 입장한 캐릭터를 배열에 추가
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(OtherActor);
			if (OverlappingNormals.Find(nc) == INDEX_NONE)
			{
				OverlappingNormals.Add(nc);
			}
		}
		else if (OtherActor->GetClass()->IsChildOf<APFBossCharacter>())
		{
			APFBossCharacter* bc = Cast<APFBossCharacter>(OtherActor);
			if (OverlappingBosses.Find(bc) == INDEX_NONE)
			{
				OverlappingBosses.Add(bc);
			}
		}
	}
}

void APFDistrict::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// 구역을 탈출한 캐릭터를 배열에서 제거
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(OtherActor);
			if (OverlappingNormals.Find(nc) != INDEX_NONE)
			{
				OverlappingNormals.Remove(nc);
			}
		}
		else if (OtherActor->GetClass()->IsChildOf<APFBossCharacter>())
		{
			APFBossCharacter* bc = Cast<APFBossCharacter>(OtherActor);
			if (OverlappingBosses.Find(bc) != INDEX_NONE)
			{
				OverlappingBosses.Remove(bc);
			}
		}
	}
}