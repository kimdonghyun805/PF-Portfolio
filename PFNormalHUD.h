// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PFNormalHUD.generated.h"

/**
 * �Ϲ� HUD Ŭ����
 * �÷��̾ �����ϴ� �Ϲ� ĳ������ ���� �� ���� ���� ���� ǥ��
 * �������Ʈ�� ���� ���۵� UserWidget ������Ʈ ���
 */
UCLASS()
class PF_API APFNormalHUD : public AHUD
{
	GENERATED_BODY()
public:
	APFNormalHUD();

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	class UClass* normalInterfaceWidgetClass;
	class UUserWidget* normalInterfaceWidget;

};
