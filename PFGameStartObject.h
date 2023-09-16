// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFObject.h"
#include "PFGameStartObject.generated.h"

/**
 * ���� ���� ������Ʈ Ŭ����
 * �ش� ������Ʈ�� �����Ͽ� ��ȣ�ۿ� ���� ��, �÷��̾ ���� ���ۿ� ����/���� �ϴ� ���·� ����
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
