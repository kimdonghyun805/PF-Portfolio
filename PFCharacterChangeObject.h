// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFObject.h"
#include "PFCharacterChangeObject.generated.h"

/**
 * ĳ���� ��ü ������Ʈ Ŭ����
 * �ش� ������Ʈ�� �����Ͽ� ��ȣ�ۿ� ���� ��, ĳ���͸� ��ü
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
