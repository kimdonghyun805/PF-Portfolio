// Fill out your copyright notice in the Description page of Project Settings.


#include "PFRifleWeaponItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PFNormalCharacter.h"
#include "PFPistolWeapon.h"

APFRifleWeaponItem::APFRifleWeaponItem()
{
	// º“√— æ∆¿Ã≈€ ∏ﬁΩ√ º≥¡§
	//StaticMesh'/Game/PF/PFWeapon/Rifle/Mesh/SM_Rifle.SM_Rifle'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFWeapon/Rifle/Mesh/SM_Rifle.SM_Rifle'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		Mesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	Mesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	Mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
}

void APFRifleWeaponItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority && OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("RifleItem Overlapped"));
		MulticastAcquired(OtherActor);
		MulticastAcquireSound(OtherActor);
		Destroy();
	}
}

void APFRifleWeaponItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFRifleWeaponItem::MulticastAcquired_Implementation(AActor* actor)
{
	SupplyAmmo(actor, 1, supplyingAmmo);
}

void APFRifleWeaponItem::SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo)
{
	APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
	if (weaponindex == 1)
	{
		// º“√— ¿Â≈∫ ∞¯±ﬁ
		nc->GetRifleWeapon()->IncreaseRemainAmmo(ammo);
	}
}