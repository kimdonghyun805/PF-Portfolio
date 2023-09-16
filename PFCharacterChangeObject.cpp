// Fill out your copyright notice in the Description page of Project Settings.


#include "PFCharacterChangeObject.h"
#include "PF.h"
#include "PFGameMode.h"
#include "PFNormalCharacter.h"
#include "PFBossCharacter.h"
#include "PFNormalPlayerState.h"
#include "PFBossPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

APFCharacterChangeObject::APFCharacterChangeObject()
{
	// �ݸ��� ���� ����
	CapsuleComponent->SetCollisionProfileName("OverlapAllDynamics");
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetCollisionResponseToChannel(NormalCollisionChannel, ECollisionResponse::ECR_Overlap);
	CapsuleComponent->SetCollisionResponseToChannel(BossCollisionChannel, ECollisionResponse::ECR_Overlap);

	// �ȳ� ������ ����� �ؽ�Ʈ ������Ʈ
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	TextRenderComponent->SetupAttachment(CapsuleComponent);
	TextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	TextRenderComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextRenderComponent->SetXScale(2.0f);
	TextRenderComponent->SetYScale(2.0f);
	TextRenderComponent->TextRenderColor = FColor::Yellow;
	TextRenderComponent->SetVisibility(true);
	TextRenderComponent->SetText(NSLOCTEXT("AnyNs", "Any", "Press 'E'\nto Change\nCharacter"));
}

void APFCharacterChangeObject::BeginPlay()
{
	Super::BeginPlay();

}

void APFCharacterChangeObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		// ��ȣ�ۿ��� ������ ĳ���� �Ǻ�
		APFNormalCharacter* changeNormal = nullptr;
		APFBossCharacter* changeBoss = nullptr;
		for (auto& actor : OverlappingActors)
		{
			if (actor->GetClass()->IsChildOf<APFNormalCharacter>())
			{
				APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
				APFNormalPlayerState* nps = nc->GetNormalPlayerState();
				if (nps != nullptr)
				{
					if (nps->IsOnInteraction())
					{
						changeNormal = nc;
					}
				}
			}
			else if (actor->GetClass()->IsChildOf<APFBossCharacter>())
			{
				APFBossCharacter* bc = Cast<APFBossCharacter>(actor);
				APFBossPlayerState* bps = bc->GetBossPlayerState();
				if (bps != nullptr)
				{
					if (bps->IsOnInteraction())
					{
						changeBoss = bc;
					}
				}
			}
		}

		// ��ȣ�ۿ��� ������ �÷��̾��� ĳ���͸� ����
		if (changeNormal != nullptr)
		{
			APFNormalPlayerState* normalPS = changeNormal->GetNormalPlayerState();
			APlayerController* normalController = normalPS->GetNormalController();
			normalPS->SetOffInteraction();
			normalPS->SetCanNotInteraction();
			OverlappingActors.Remove(changeNormal);
			APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
			gameMode->ChangeNormalToBoss(normalController);
		}
		else if (changeBoss != nullptr)
		{
			APFBossPlayerState* bossPS = changeBoss->GetBossPlayerState();
			APlayerController* bossController = bossPS->GetBossController();
			bossPS->SetOffInteraction();
			bossPS->SetCanNotInteraction();
			OverlappingActors.Remove(changeBoss);
			APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
			gameMode->ChangeBossToNormal(bossController);
		}
	}
}

void APFCharacterChangeObject::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// ĳ���Ͱ� �ش� ������Ʈ�� ������ ��, ��ȣ�ۿ� ������ ���·� ����
	if (GetLocalRole() == ROLE_Authority)
	{
		Super::ActorBeginOverlap(OverlappedActor, OtherActor);
		if (OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(OtherActor);
			APFNormalPlayerState* nps = nc->GetNormalPlayerState();
			if (nps != nullptr)
			{
				nps->SetCanInteraction();
			}
		}
		else if (OtherActor->GetClass()->IsChildOf<APFBossCharacter>())
		{
			APFBossCharacter* bc = Cast<APFBossCharacter>(OtherActor);
			APFBossPlayerState* bps = bc->GetBossPlayerState();
			if (bps != nullptr)
			{
				bps->SetCanInteraction();
			}
		}
	}
}

void APFCharacterChangeObject::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// ĳ���Ͱ� �ش� ������Ʈ�� ��ġ�� �ʾ��� ��, ��ȣ�ۿ� �Ұ����� ���·� ����
	if (GetLocalRole() == ROLE_Authority)
	{
		Super::ActorEndOverlap(OverlappedActor, OtherActor);
		if (OtherActor->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(OtherActor);
			APFNormalPlayerState* nps = nc->GetNormalPlayerState();
			if (nps != nullptr)
			{
				nps->SetCanNotInteraction();
			}
		}
		else if (OtherActor->GetClass()->IsChildOf<APFBossCharacter>())
		{
			APFBossCharacter* bc = Cast<APFBossCharacter>(OtherActor);
			APFBossPlayerState* bps = bc->GetBossPlayerState();
			if (bps != nullptr)
			{
				bps->SetCanNotInteraction();
			}
		}
	}
}
