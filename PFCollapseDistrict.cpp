// Fill out your copyright notice in the Description page of Project Settings.

#include "PFCollapseDistrict.h"
#include "PF.h"
#include "PFGameMode.h"
#include "PFNormalCharacter.h"
#include "PFBossCharacter.h"
#include "PFCoreObject.h"
#include "PFNormalSpawner.h"
#include "PFWeaponSpawner.h"
#include "PFRecoverySpawner.h"
#include "PFParticleSpawner.h"
#include "PFItem.h"
#include "Components/BoxComponent.h"

APFCollapseDistrict::APFCollapseDistrict()
{
	bIsCollapsed = false;
	collapsingTime = maxCollapsingTime;
	damageTime = maxDamageTime;
	normalSpawnerDestroyTime = maxNormalSpawnerDestroyTime;
	itemSpawnerDestroyTime = maxItemSpawnerDestroyTime;
	bIsItemDestroyed = false;
}

void APFCollapseDistrict::BeginPlay()
{
	Super::BeginPlay();
	if (CoreObject != nullptr)
	{
		CoreObject->SetOwningDistrict(this);
	}
}

void APFCollapseDistrict::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (bIsCollapsed)
		{
			if (collapsingTime > 0.0f) 
			{
				// 60�� ���� �޽� ����
				Mesh->AddRelativeLocation(FVector(0.0f, 0.0f, -5.0f * DeltaTime));
				collapsingTime -= DeltaTime;
			}
			if (!(NormalSpawners.IsEmpty())) 
			{
				// 2�� �������� ������ �����ϴ� �Ϲ� ĳ���� ������ ����
				if (normalSpawnerDestroyTime <= 0.0f)
				{
					DestroyRandomNormalSpawner();
					normalSpawnerDestroyTime = maxNormalSpawnerDestroyTime;
				}
				normalSpawnerDestroyTime -= DeltaTime;
			}
			if (!(WeaponSpawners.IsEmpty()) || !(RecoverySpawners.IsEmpty())) 
			{
				// 10�� �������� ������ �����ϴ� ������ ������ ����
				if (itemSpawnerDestroyTime <= 0.0f)
				{
					DestroyRandomWeaponSpawner();
					DestroyRandomRecoverySpawner();
					itemSpawnerDestroyTime = maxItemSpawnerDestroyTime;
				}
				itemSpawnerDestroyTime -= DeltaTime;
			}
			if (!bIsItemDestroyed)
			{
				if ((NormalSpawners.IsEmpty() && WeaponSpawners.IsEmpty()) && RecoverySpawners.IsEmpty())
				{
					// ��� ������ ���� ��, ������ �����ִ� ������ ����
					TSubclassOf<APFItem> itemClass = APFItem::StaticClass();
					TArray<AActor*> actors;
					GetBoxComponent()->GetOverlappingActors(actors, itemClass);
					for (auto& a : actors)
					{
						a->Destroy();
					}

					bIsItemDestroyed = true;
				}
			}

			if (!(OverlappingNormals.IsEmpty())) 
			{
				// 1�� �������� ������ �����ϴ� �Ϲ� ĳ���Ϳ��� ������
				if (damageTime <= 0.0f)
				{
					APFNormalCharacter* removeTargets[NumMaxPlayer];
					uint8 removeTargetIndex = 0;
					for (auto& nc : OverlappingNormals)
					{
						if (nc->GetHealth() > tickDamage)
						{
							FDamageEvent collapseTickDamageEvent(UDamageType::StaticClass());
							nc->TakeDamage(tickDamage, collapseTickDamageEvent, nullptr, this);
						}
						else
						{
							removeTargets[removeTargetIndex] = nc;
							++removeTargetIndex;
						}
					}
					if (removeTargetIndex > 0)
					{
						for (uint8 i = 0; i < removeTargetIndex; ++i)
						{
							OverlappingNormals.Remove(removeTargets[i]);
							FDamageEvent collapseTickDamageEvent(UDamageType::StaticClass());
							removeTargets[i]->TakeDamage(tickDamage, collapseTickDamageEvent, nullptr, this);
							removeTargets[i] = nullptr;
						}
					}
					damageTime = maxDamageTime;
				}
				damageTime -= DeltaTime;
			}
		}
	}
}

void APFCollapseDistrict::CoreDestroyed()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	Collapse();
	GetWorld()->GetAuthGameMode<APFGameMode>()->CoreDestroyed(CoreObject);
}

void APFCollapseDistrict::Collapse()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	bIsCollapsed = true;
	CoreObject = nullptr;
	// �ı� ��ƼŬ Ȱ��ȭ
	for (auto& ps : ParticleSpawners)
	{
		ps->ActivateParticle();
	}
}

void APFCollapseDistrict::DestroyRandomNormalSpawner()
{
	int32 num = NormalSpawners.Num();
	if (num > 0)
	{
		int32 rand = FMath::RandRange(0, num - 1);
		APFNormalSpawner* ns = NormalSpawners[rand];
		NormalSpawners.Remove(ns);
		ns->Destroy();
	}
}

void APFCollapseDistrict::DestroyRandomWeaponSpawner()
{
	int32 num = WeaponSpawners.Num();
	if (num > 0)
	{
		int32 rand = FMath::RandRange(0, num - 1);
		APFWeaponSpawner* ws = WeaponSpawners[rand];
		WeaponSpawners.Remove(ws);
		ws->Destroy();
	}
}

void APFCollapseDistrict::DestroyRandomRecoverySpawner()
{
	int32 num = RecoverySpawners.Num();
	if (num > 0)
	{
		int32 rand = FMath::RandRange(0, num - 1);
		APFRecoverySpawner* rs = RecoverySpawners[rand];
		RecoverySpawners.Remove(rs);
		rs->Destroy();
	}
}