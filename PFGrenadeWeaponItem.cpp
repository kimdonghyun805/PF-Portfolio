// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGrenadeWeaponItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"
#include "PFGrenadeWeapon.h"

APFGrenadeWeaponItem::APFGrenadeWeaponItem()
{
	// 수류탄 아이템 메시 설정
	//StaticMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade.SM_grenade'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade.SM_grenade'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	Mesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
	Mesh->AddRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
	Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
}

void APFGrenadeWeaponItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		MulticastAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFGrenadeWeaponItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFGrenadeWeaponItem::MulticastAcquired_Implementation(AActor* actor)
{
	SupplyAmmo(actor, 3, supplyingAmmo);
}

void APFGrenadeWeaponItem::SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (weaponindex == 3)
	{
		// 수류탄 장탄 증가
		nc->GetGrenadeWeapon()->IncreaseRemainAmmo(ammo);
	}
}