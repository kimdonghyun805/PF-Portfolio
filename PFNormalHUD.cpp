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
	// ���� ��� �������� _C  �߰��ؾ� �ε� ����
	
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
	// HUD ������ �������� �ʾҴٸ�, ������ �� ȭ�鿡 ǥ��
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
	// ���� ������ ȭ�鿡�� ����
	Super::EndPlay(EndPlayReason);
	if (normalInterfaceWidget != nullptr && normalInterfaceWidget->IsInViewport())
	{
		normalInterfaceWidget->RemoveFromViewport();
	}
}
