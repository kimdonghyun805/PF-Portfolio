// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFItem.h"
#include "PFRecoveryItem.generated.h"

/**
 * 회복 아이템 클래스
 * 회복 아이템의 공통 속성 및 함수 설정
 * 회복 아이템은 체력, 보호막, 목숨의 3종류로 구성
 */
UCLASS()
class PF_API APFRecoveryItem : public APFItem
{
	GENERATED_BODY()
	
public:
	APFRecoveryItem();

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

public:
	// 회복 아이템으로 변경되는 변수는 PlayerState에서 보유하므로, Server 함수 실행
	virtual void ServerAcquired_Implementation(AActor* actor) override;
	virtual void Recover(AActor* actor, const int& itemtype, const float& value);
};
