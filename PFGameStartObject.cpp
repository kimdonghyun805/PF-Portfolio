// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameStartObject.h"
#include "PF.h"
#include "PFNormalCharacter.h"
#include "PFBossCharacter.h"
#include "PFNormalPlayerState.h"
#include "PFBossPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

APFGameStartObject::APFGameStartObject()
{
	// �ݸ��� ����
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
	TextRenderComponent->TextRenderColor = FColor::Green;
	TextRenderComponent->SetVisibility(true);
	TextRenderComponent->SetText(NSLOCTEXT("AnyNs", "Any", "Press 'E'\nto Start\nthe Game"));
}

void APFGameStartObject::BeginPlay()
{
	Super::BeginPlay();

}

void APFGameStartObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		// ��ȣ�ۿ��� ������ ĳ������ ���� ���� ���Ǹ� ����
		for (auto& actor : OverlappingActors)
		{
			if (actor->GetClass()->IsChildOf<APFNormalCharacter>())
			{
				APFNormalCharacter* nc = Cast<APFNormalCharacter>(actor);
				APFNormalPlayerState* nps = nc->GetNormalPlayerState();
				if (nps != nullptr)
				{
					APlayerController* npc = nps->GetNormalController();
					if (nps->IsOnInteraction())
					{
						if (nps->GetAgreeToStart())
						{
							nps->SetDisagreeToStart();
						}
						else
						{
							nps->SetAgreeToStart();
						}
						nps->SetOffInteraction();
						nps->SetCanNotInteraction();
					}
				}
			}
			else if (actor->GetClass()->IsChildOf<APFBossCharacter>())
			{
				APFBossCharacter* bc = Cast<APFBossCharacter>(actor);
				APFBossPlayerState* bps = bc->GetBossPlayerState();
				if (bps != nullptr)
				{
					APlayerController* bpc = bps->GetBossController();
					if (bps->IsOnInteraction())
					{
						if (bps->GetAgreeToStart())
						{
							bps->SetDisagreeToStart();
						}
						else
						{
							bps->SetAgreeToStart();
						}
						bps->SetOffInteraction();
						bps->SetCanNotInteraction();
					}
				}
			}
		}
	}
}

void APFGameStartObject::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
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

void APFGameStartObject::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
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
