// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PFBossHUD.generated.h"

/**
 * ���� HUD Ŭ����
 * �÷��̾ �����ϴ� ���� ĳ������ ���� �� ���� ���� ���� ǥ��
 * �������Ʈ�� ���� ���۵� UserWidget ������Ʈ ���
 */
UCLASS()
class PF_API APFBossHUD : public AHUD
{
	GENERATED_BODY()
public :
	APFBossHUD();

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected :
	class UClass* bossInterfaceWidgetClass;
	class UUserWidget* bossInterfaceWidget;

};
