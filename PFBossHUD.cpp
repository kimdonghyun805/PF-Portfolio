// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBossHUD.h"
#include "PF.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"

APFBossHUD::APFBossHUD()
{
	// ���� ��� �������� _C  �߰��ؾ� �ε� ����
	static ConstructorHelpers::FClassFinder<UUserWidget> bossInterfaceWidgetObj(
		TEXT("WidgetBlueprint'/Game/PF/PFUI/BossInterface.BossInterface_C'"));
	if (bossInterfaceWidgetObj.Succeeded())
	{
		bossInterfaceWidgetClass = bossInterfaceWidgetObj.Class;
	}
	else
	{
		bossInterfaceWidgetClass = nullptr;
	}
	bossInterfaceWidget = nullptr;

}

void APFBossHUD::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFBossHUD::DrawHUD()
{
	// HUD ������ �������� �ʾҴٸ�, ������ �� ȭ�鿡 ǥ��
	if (bossInterfaceWidget == nullptr)
	{
		if (bossInterfaceWidgetClass != nullptr)
		{
			bossInterfaceWidget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), bossInterfaceWidgetClass);
			bossInterfaceWidget->AddToViewport();
		}
	}
	
}

void APFBossHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ���� ������ ȭ�鿡�� ����
	Super::EndPlay(EndPlayReason);
	if (bossInterfaceWidget != nullptr && bossInterfaceWidget->IsInViewport())
	{
		bossInterfaceWidget->RemoveFromViewport();
	}
}
