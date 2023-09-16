// Fill out your copyright notice in the Description page of Project Settings.


#include "PFShieldRecoveryItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"

APFShieldRecoveryItem::APFShieldRecoveryItem()
{
	// 보호막 아이템 메시 설정
	//StaticMesh'/Game/PF/PFItem/SM_Pickup_Defence.SM_Pickup_Defence'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFItem/SM_Pickup_Defence.SM_Pickup_Defence'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	//Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
	Mesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
}

void APFShieldRecoveryItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		ServerAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFShieldRecoveryItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFShieldRecoveryItem::ServerAcquired_Implementation(AActor* actor)
{
	Recover(actor, 1, recoverShield);
}

void APFShieldRecoveryItem::Recover(AActor* actor, const int& itemtype, const float& value)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (itemtype == 1)
	{
		// 보호막 충전
		nc->IncreaseShield(value);
	}
}
