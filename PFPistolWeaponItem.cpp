// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPistolWeaponItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"
#include "PFPistolWeapon.h"

APFPistolWeaponItem::APFPistolWeaponItem()
{
	// ±«√— æ∆¿Ã≈€ ∏ﬁΩ√ º≥¡§
	//StaticMesh'/Game/PF/PFWeapon/Pistol/Mesh/SM_Pistol.SM_Pistol'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFWeapon/Pistol/Mesh/SM_Pistol.SM_Pistol'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	Mesh->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
}

void APFPistolWeaponItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		MulticastAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFPistolWeaponItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFPistolWeaponItem::MulticastAcquired_Implementation(AActor* actor)
{
	SupplyAmmo(actor, 0, supplyingAmmo);
}

void APFPistolWeaponItem::SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (weaponindex == 0)
	{
		// ±«√— ¿Â≈∫ ∞¯±ﬁ
		nc->GetPistolWeapon()->IncreaseRemainAmmo(ammo);
	}
}