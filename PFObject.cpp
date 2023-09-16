// Fill out your copyright notice in the Description page of Project Settings.


#include "PFObject.h"
#include "PF.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

APFObject::APFObject()
{
	PrimaryActorTick.bCanEverTick = true;
	OnActorBeginOverlap.AddDynamic(this, &APFObject::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APFObject::ActorEndOverlap);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetupAttachment(RootComponent);
	CapsuleComponent->SetCapsuleSize(82.5f, 144.0f);
	CapsuleComponent->SetCollisionProfileName("OverlapAllDynamics");
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->SetCollisionResponseToChannel(NormalCollisionChannel, ECollisionResponse::ECR_Overlap);
	CapsuleComponent->SetCollisionResponseToChannel(BossCollisionChannel, ECollisionResponse::ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	Mesh->SetupAttachment(CapsuleComponent);
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;
	// 바닥 메시 설정
	//StaticMesh'/Game/PF/PFSpawnPad/Mesh/SM_launchpad_Hex.SM_launchpad_Hex'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder0(
		TEXT("StaticMesh'/Game/PF/PFSpawnPad/Mesh/SM_launchpad_Hex.SM_launchpad_Hex'"));
	if (staticMeshObjectFinder0.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder0.Object);
	}
	//Mesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -140.0f));
	//Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));

	InteractionSound = nullptr;

	bReplicates = true;
}

void APFObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APFObject::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappingActors.Find(OtherActor) == INDEX_NONE)
	{
		OverlappingActors.Add(OtherActor);
	}
}

void APFObject::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappingActors.Find(OtherActor) != INDEX_NONE)
	{
		OverlappingActors.Remove(OtherActor);
	}
}