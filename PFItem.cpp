// Fill out your copyright notice in the Description page of Project Settings.


#include "PFItem.h"
#include "PF.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APFItem::APFItem()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &APFItem::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APFItem::ActorEndOverlap);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ItemCapsule"));
	SphereComponent->SetSphereRadius(55.0f);
	SphereComponent->SetCollisionProfileName("OverlapAllDynamics");
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionObjectType(ItemCollisionChannel);
	SphereComponent->SetCollisionResponseToChannel(NormalCollisionChannel, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(BossCollisionChannel, ECollisionResponse::ECR_Overlap);
	RootComponent = SphereComponent;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	Mesh->SetupAttachment(SphereComponent);
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	EffectNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffectNiagara"));
	EffectNiagara->bAutoActivate = true;
	EffectNiagara->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	EffectNiagara->SetupAttachment(SphereComponent);
	//EffectNiagara->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	EffectNiagara->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));

	EffectMaterial = nullptr;
	AcquireSound = nullptr;

	bReplicates = true;
}

void APFItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 메시 회전 연출
	Mesh->AddRelativeRotation(FRotator(0.0f, 90.0f * DeltaTime, 0.0f));
}

void APFItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFItem::MulticastAcquired_Implementation(AActor* actor)
{
	//
}

void APFItem::ServerAcquired_Implementation(AActor* actor)
{
	//
}

void APFItem::MulticastAcquireSound_Implementation(AActor* actor)
{
	UGameplayStatics::PlaySoundAtLocation(actor, AcquireSound, GetActorLocation());
}