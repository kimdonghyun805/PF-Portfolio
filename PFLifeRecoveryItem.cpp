// Fill out your copyright notice in the Description page of Project Settings.


#include "PFLifeRecoveryItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"

APFLifeRecoveryItem::APFLifeRecoveryItem()
{
	// 목숨 아이템 메시 설정
	//StaticMesh'/Game/PF/PFItem/SM_Coin_1.SM_Coin_1'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFItem/SM_Coin_1.SM_Coin_1'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	//Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
	Mesh->SetRelativeScale3D(FVector(3.0f, 6.0f, 6.0f));
}

void APFLifeRecoveryItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		ServerAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFLifeRecoveryItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFLifeRecoveryItem::ServerAcquired_Implementation(AActor* actor)
{
	Recover(actor, 2, recoverLife);
}

void APFLifeRecoveryItem::Recover(AActor* actor, const int& itemtype, const float& value)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (itemtype == 2)
	{
		// 캐릭터의 목숨을 증가
		nc->IncreaseLife((int)value);
	}
}
