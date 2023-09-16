// Fill out your copyright notice in the Description page of Project Settings.


#include "PFNormalHUD.h"
#include "PF.h"
#include "PFGameMode.h"
#include "PFNormalCharacter.h"
#include "PFNormalPlayerState.h"
#include "PFWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"


APFNormalHUD::APFNormalHUD()
{
	// 위젯 경로 마지막에 _C  추가해야 로딩 성공
	
	//WidgetBlueprint'/Game/PF/PFUI/NormalInterface.NormalInterface'
	static ConstructorHelpers::FClassFinder<UUserWidget> normalInterfaceWidgetObj(
		TEXT("WidgetBlueprint'/Game/PF/PFUI/NormalInterface.NormalInterface_C'"));
	if (normalInterfaceWidgetObj.Succeeded())
	{
		normalInterfaceWidgetClass = normalInterfaceWidgetObj.Class;
	}
	else
	{
		normalInterfaceWidgetClass = nullptr;
	}
	normalInterfaceWidget = nullptr;

}

void APFNormalHUD::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFNormalHUD::DrawHUD()
{
	// HUD 위젯이 생성되지 않았다면, 생성한 후 화면에 표시
	if (normalInterfaceWidget == nullptr)
	{
		if (normalInterfaceWidgetClass != nullptr)
		{
			normalInterfaceWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), normalInterfaceWidgetClass);
			normalInterfaceWidget->AddToViewport();
		}
	}

}

void APFNormalHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 기존 위젯을 화면에서 제거
	Super::EndPlay(EndPlayReason);
	if (normalInterfaceWidget != nullptr && normalInterfaceWidget->IsInViewport())
	{
		normalInterfaceWidget->RemoveFromViewport();
	}
}
