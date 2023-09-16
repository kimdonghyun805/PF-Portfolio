// Fill out your copyright notice in the Description page of Project Settings.


#include "PFWeaponItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APFWeaponItem::APFWeaponItem()
{
	// 아이템 이펙트 머티리얼 설정
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Red.MT_Wind_Red'
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Red.MT_Wind_Red'"));
	if (materialObj.Succeeded())
	{
		EffectMaterial = materialObj.Object;
	}
	// 아이템 이펙트 설정
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Item.Item'
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> niagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Item.Item'"));
	if (niagaraObj.Succeeded())
	{
		EffectNiagara->SetAsset(niagaraObj.Object);
		if (EffectMaterial != nullptr)
		{
			EffectNiagara->SetVariableMaterial(FName("MeshMaterial"), EffectMaterial);
		}
	}
	// 아이템 획득 사운드 설정
	//SoundCue'/Game/PF/Audio/Item/WeaponAcquire.WeaponAcquire'
	static ConstructorHelpers::FObjectFinder<USoundCue> soundObj(
		TEXT("SoundCue'/Game/PF/Audio/Item/WeaponAcquire.WeaponAcquire'"));
	if (soundObj.Succeeded())
	{
		AcquireSound = soundObj.Object;
	}
}

void APFWeaponItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFWeaponItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFWeaponItem::MulticastAcquired_Implementation(AActor* actor)
{
	//
}

void APFWeaponItem::SupplyAmmo(AActor* actor, const int& weaponindex, const int& ammo)
{
	//
}