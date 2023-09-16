// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFObject.h"
#include "PFCharacterChangeObject.generated.h"

/**
 * 캐릭터 교체 오브젝트 클래스
 * 해당 오브젝트에 접근하여 상호작용 수행 시, 캐릭터를 교체
 */
UCLASS()
class PF_API APFCharacterChangeObject : public APFObject
{
	GENERATED_BODY()

public:
	APFCharacterChangeObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* TextRenderComponent;

protected :
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor) override;
};
