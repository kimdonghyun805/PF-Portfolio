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
	// 위젯 경로 마지막에 _C  추가해야 로딩 성공
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
	// HUD 위젯이 생성되지 않았다면, 생성한 후 화면에 표시
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
	// 기존 위젯을 화면에서 제거
	Super::EndPlay(EndPlayReason);
	if (bossInterfaceWidget != nullptr && bossInterfaceWidget->IsInViewport())
	{
		bossInterfaceWidget->RemoveFromViewport();
	}
}
