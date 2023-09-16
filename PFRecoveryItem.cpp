// Fill out your copyright notice in the Description page of Project Settings.


#include "PFRecoveryItem.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APFRecoveryItem::APFRecoveryItem()
{
	// ������ ����Ʈ ��Ƽ���� ����
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Green.MT_Wind_Green'
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Green.MT_Wind_Green'"));
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