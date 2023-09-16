// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PFNormalHUD.generated.h"

/**
 * 일반 HUD 클래스
 * 플레이어가 조종하는 일반 캐릭터의 정보 및 게임 진행 정보 표시
 * 블루프린트를 통해 제작된 UserWidget 오브젝트 사용
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
