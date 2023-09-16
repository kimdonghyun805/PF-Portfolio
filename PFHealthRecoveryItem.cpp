// Fill out your copyright notice in the Description page of Project Settings.


#include "PFHealthRecoveryItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"

APFHealthRecoveryItem::APFHealthRecoveryItem()
{
	// 체력 아이템 메시 설정
	//StaticMesh'/Game/PF/PFItem/SM_Pickup_Health.SM_Pickup_Health'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFItem/SM_Pickup_Health.SM_Pickup_Health'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	//Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
	Mesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
}

void APFHealthRecoveryItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		ServerAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFHealthRecoveryItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFHealthRecoveryItem::ServerAcquired_Implementation(AActor* actor)
{
	Recover(actor, 0, recoverHealth);
}

void APFHealthRecoveryItem::Recover(AActor* actor, const int& itemtype, const float& value)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (itemtype == 0)
	{
		// 체력을 회복
		nc->IncreaseHealth(value);
	}
}