// Fill out your copyright notice in the Description page of Project Settings.


#include "PFWeaponItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APFWeaponItem::APFWeaponItem()
{
	// ������ ����Ʈ ��Ƽ���� ����
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Red.MT_Wind_Red'
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Red.MT_Wind_Red'"));
	if (materialObj.Succeeded())
	{
		EffectMaterial = materialObj.Object;
	}
	// ������ ����Ʈ ����
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
	// ������ ȹ�� ���� ����
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