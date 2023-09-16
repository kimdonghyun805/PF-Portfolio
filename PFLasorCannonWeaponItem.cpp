// Fill out your copyright notice in the Description page of Project Settings.


#include "PFLasorCannonWeaponItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"
#include "PFPistolWeapon.h"

APFLasorCannonWeaponItem::APFLasorCannonWeaponItem()
{
	// 레이저 캐논 아이템 메시 설정
	//StaticMesh'/Game/PF/PFWeapon/LasorCannon/Mesh/SM_Shotgun.SM_Shotgun'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFWeapon/LasorCannon/Mesh/SM_Shotgun.SM_Shotgun'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	Mesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
}

void APFLasorCannonWeaponItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		MulticastAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFLasorCannonWeaponItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFLasorCannonWeaponItem::MulticastAcquired_Implementation(AActor* actor)
{
	SupplyAmmo(actor, 2, supplyingAmmo);
}

void APFLasorCannonWeaponItem::SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (weaponindex == 2)
	{
		// 레이저 캐논 장탄 증가
		nc->GetLasorCannonWeapon()->IncreaseRemainAmmo(ammo);
	}
}