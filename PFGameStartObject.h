// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFObject.h"
#include "PFGameStartObject.generated.h"

/**
 * 게임 시작 오브젝트 클래스
 * 해당 오브젝트에 접근하여 상호작용 수행 시, 플레이어를 게임 시작에 동의/비동의 하는 상태로 설정
 */
UCLASS()
class PF_API APFGameStartObject : public APFObject
{
	GENERATED_BODY()

public:
	APFGameStartObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* TextRenderComponent;

protected:
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
};
