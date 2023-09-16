// Fill out your copyright notice in the Description page of Project Settings.


#include "PFCoreObject.h"
#include "PF.h"
#include "PFCollapseDistrict.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

APFCoreObject::APFCoreObject()
{
	// 콜리전 수정
	CapsuleComponent->SetCollisionProfileName("BlockAll");
	CapsuleComponent->SetCollisionObjectType(CoreCollisionChannel);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;

	// 코어 메시 생성
	CoreMeshCenter = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMeshCenter"));
	CoreMeshCenter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CoreMeshCenter->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CoreMeshCenter->SetupAttachment(CapsuleComponent);
	CoreMeshCenter->bCastDynamicShadow = false;
	CoreMeshCenter->CastShadow = false;
	CoreMeshTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMeshTop"));
	CoreMeshTop->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CoreMeshTop->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CoreMeshTop->SetupAttachment(CapsuleComponent);
	CoreMeshTop->bCastDynamicShadow = false;
	CoreMeshTop->CastShadow = false;
	CoreMeshBottom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMeshBottom"));
	CoreMeshBottom->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CoreMeshBottom->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CoreMeshBottom->SetupAttachment(CapsuleComponent);
	CoreMeshBottom->bCastDynamicShadow = false;
	CoreMeshBottom->CastShadow = false;
	
	// 코어 메시 설정
	//StaticMesh'/Game/PF/PFObject/SM_Pickup_Gem.SM_Pickup_Gem'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder1(
		TEXT("StaticMesh'/Game/PF/PFObject/SM_Pickup_Gem.SM_Pickup_Gem'"));
	if (staticMeshObjectFinder1.Succeeded())
	{
		CoreMeshCenter->SetStaticMesh(staticMeshObjectFinder1.Object);
		CoreMeshTop->SetStaticMesh(staticMeshObjectFinder1.Object);
		CoreMeshBottom->SetStaticMesh(staticMeshObjectFinder1.Object);
	}
	CoreMeshCenter->SetRelativeScale3D(FVector(6.0f, 6.0f, 3.0f));
	CoreMeshCenter->SetRelativeLocation(FVector(0.0f, 0.0f, -75.0f));
	//CoreMeshCenter->SetRelativeRotation(FRotator(0.0f, 0.0f, -30.0f));
	CoreMeshTop->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	CoreMeshBottom->SetRelativeLocation(FVector(0.0f, 0.0f, -120.0f));

	// 파괴 사운드 설정
	//SoundCue'/Game/PF/Audio/Object/CoreDestruction.CoreDestruction'
	static ConstructorHelpers::FObjectFinder<USoundCue> soundObj(
		TEXT("SoundCue'/Game/PF/Audio/Object/CoreDestruction.CoreDestruction'"));
	if (soundObj.Succeeded())
	{
		InteractionSound = soundObj.Object;
	}
	// 파괴 이펙트 생성
	DestructionParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DestructionParticle"));
	//ParticleSystem'/Game/PF/Effect/Particle/P_Explosion.P_Explosion'
	static ConstructorHelpers::FObjectFinder<UParticleSystem> destructionParticleFinder(
		TEXT("ParticleSystem'/Game/PF/Effect/Particle/P_Explosion.P_Explosion'"));
	if (destructionParticleFinder.Succeeded())
	{
		DestructionParticle->SetTemplate(destructionParticleFinder.Object);
		DestructionParticle->SetupAttachment(CoreMeshCenter);
		DestructionParticle->bAutoActivate = false;
	}

	// 기본 변수 설정
	health = maxHealth;
}

// Called every frame
void APFCoreObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 코어 메시를 회전
	CoreMeshCenter->AddRelativeRotation(FRotator(0.0f, 90.0f * DeltaTime, 0.0f));
	CoreMeshTop->AddRelativeRotation(FRotator(0.0f, -90.0f * DeltaTime, 0.0f));
	CoreMeshBottom->AddRelativeRotation(FRotator(0.0f, -90.0f * DeltaTime, 0.0f));
}

float APFCoreObject::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
	AController* EventIntigator, AActor* DamageCauser)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		health -= Damage;
		if (health <= 0.0f)
		{
			PlayDestructionEffect();
			// gamemode에서 reinforcelevel을 증가
			if (OwningDistrict != nullptr)
			{
				// 구역에 코어의 파괴를 전달
				OwningDistrict->CoreDestroyed();
			}
			Destroy();
		}
	}
	return Damage;
}

void APFCoreObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFCoreObject, health);

}

void APFCoreObject::PlayDestructionEffect_Implementation()
{
	if (DestructionParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DestructionParticle->Template, GetActorLocation(), FRotator(), FVector(8.0f, 8.0f, 8.0f));
	}
	if (InteractionSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
	}
}

void APFCoreObject::SetOwningDistrict(APFCollapseDistrict* district)
{
	OwningDistrict = district;
}