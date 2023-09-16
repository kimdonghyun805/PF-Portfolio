// Fill out your copyright notice in the Description page of Project Settings.


#include "PFCenterDistrict.h"
#include "PF.h"
#include "PFBossCharacter.h"

APFCenterDistrict::APFCenterDistrict()
{
	healTime = maxHealTime;
}

void APFCenterDistrict::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFCenterDistrict::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!(OverlappingBosses.IsEmpty())) // 1초 간격으로 구역에 존재하는 보스 캐릭터 체력 회복
		{
			if (healTime <= 0.0f)
			{
				for (auto& bc : OverlappingBosses)
				{
					if (bc->GetHealth() < bc->GetMaxHealth())
					{
						bc->IncreaseHealth(healingHealth);
					}
				}
				healTime = maxHealTime;
			}
			healTime -= DeltaTime;
		}
	}
}
