// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFSpawner.h"
#include "PFNormalSpawner.generated.h"

/**
 * 일반 스포너 클래스
 * 해당 오브젝트의 위치로 보스 캐릭터의 스폰 위치 지정
 */
UCLASS()
class PF_API APFNormalSpawner : public APFSpawner
{
	GENERATED_BODY()
public :
	APFNormalSpawner();
};
