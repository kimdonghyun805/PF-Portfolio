// Fill out your copyright notice in the Description page of Project Settings.


#include "PFRecoveryItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APFRecoveryItem::APFRecoveryItem()
{
	// 아이템 이펙트 머티리얼 설정
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Green.MT_Wind_Green'
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Green.MT_Wind_Green'"));
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
	//SoundCue'/Game/PF/Audio/Item/RecoveryAcquire.RecoveryAcquire'
	static ConstructorHelpers::FObjectFinder<USoundCue> soundObj(
		TEXT("SoundCue'/Game/PF/Audio/Item/RecoveryAcquire.RecoveryAcquire'"));
	if (soundObj.Succeeded())
	{
		AcquireSound = soundObj.Object;
	}
}

void APFRecoveryItem::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFRecoveryItem::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//
}

void APFRecoveryItem::ServerAcquired_Implementation(AActor* actor)
{
	//
}

void APFRecoveryItem::Recover(AActor* actor, const int& itemtype, const float& value)
{
	//
}