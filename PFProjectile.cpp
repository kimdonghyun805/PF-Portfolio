// Copyright Epic Games, Inc. All Rights Reserved.

#include "PFProjectile.h"
#include "PF.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "PFCharacter.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

APFProjectile::APFProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CollisionComp->InitCapsuleSize(5.5f, 9.6f);
	CollisionComp->BodyInstance.SetCollisionProfileName("BlockAll");
	CollisionComp->OnComponentHit.AddDynamic(this, &APFProjectile::OnHit);
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	// �߻�ü ������ ����
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// �߻�ü �޽� ����
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	//StaticMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade.SM_grenade'
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshObjectFinder(
		TEXT("StaticMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade.SM_grenade'"));
	if (staticMeshObjectFinder.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(staticMeshObjectFinder.Object);
	}
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
	ProjectileMesh->SetVisibility(true);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// ���� ����Ʈ ����
	BlastParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BlastParticle"));
	//ParticleSystem'/Game/PF/Effect/Particle/P_Explosion.P_Explosion'
	static ConstructorHelpers::FObjectFinder<UParticleSystem> blastParticleFinder(
		TEXT("ParticleSystem'/Game/PF/Effect/Particle/P_Explosion.P_Explosion'"));
	if (blastParticleFinder.Succeeded())
	{
		BlastParticle->SetTemplate(blastParticleFinder.Object);
		BlastParticle->SetupAttachment(RootComponent);
		BlastParticle->bAutoActivate = false;
	}
	// ���� ���� ����
	// SoundCue'/Game/PF/Audio/Object/BlastSound.BlastSound'
	static ConstructorHelpers::FObjectFinder<USoundCue> blastSoundFinder(
		TEXT("SoundCue'/Game/PF/Audio/Object/BlastSound.BlastSound'"));
	if (blastSoundFinder.Succeeded())
	{
		BlastSound = blastSoundFinder.Object;
	}

	// �߻�ü ����
	maxLifetime = 4.0f;
	lifetime = maxLifetime;

	InitialLifeSpan = 4.2f;
	bReplicates = true;
}

void APFProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (lifetime > 0.0f)
		{
			lifetime -= DeltaTime;
		}
		else
		{
			// ������ �� �� �߻�ü�� �����Ͽ� ���ظ� �ְ� �Ҹ�
			ServerBlast();
			Destroy();
		}
	}

}

void APFProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		// ���� ĳ���Ϳ� �浹�� ���, ��� �����Ͽ� ���ظ� �ְ� �Ҹ�
		if (OtherActor->GetClass()->IsChildOf<APFBossCharacter>() && GetLocalRole() == ROLE_Authority)
		{
			ServerBlast();
			Destroy();
		}		
	}
}

void APFProjectile::ServerBlast_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	FVector start = GetActorLocation();
	FVector end = GetActorLocation() + FVector(0.0f, 0.0f, 1.0f);
	float radius = 800.0f;
	float damage = 200.0f;

	// ��ü�� sweep ����Ͽ� ���� ������ �Ϲ�, ���� ĳ���͸� Ž��
	FCollisionObjectQueryParams targetObj;
	targetObj.AddObjectTypesToQuery(NormalCollisionChannel);
	targetObj.AddObjectTypesToQuery(BossCollisionChannel);
	FCollisionQueryParams thisObj(FName(TEXT("SweepSphere")), true, this);
	TArray<FHitResult> hitResult;
	GetWorld()->SweepMultiByObjectType(hitResult, start, end, FQuat(), targetObj, FCollisionShape::MakeSphere(radius), thisObj);
	for (auto& hit : hitResult)
	{
		if (hit.GetActor()->GetClass()->IsChildOf<APFBossCharacter>()) // ������ ������Ʈ�� Ÿ�� ������� Ȯ��
		{
			// ���� ĳ���Ϳ��� ���ظ� ��
			APFBossCharacter* bc = Cast<APFBossCharacter>(hit.GetActor());
			FDamageEvent projectileEvent(UDamageType::StaticClass());
			bc->TakeDamage(damage, projectileEvent, projectileOwner->GetController(), projectileOwner);
			//UE_LOG(LogTemp, Warning, TEXT("Projectile Attack Target %s"), *(bc->GetName()));
		}
		if (hit.GetActor()->GetClass()->IsChildOf<APFCharacter>())
		{
			// �Ϲ�, ���� ĳ���͸� �˹�
			APFCharacter* pfc = Cast<APFCharacter>(hit.GetActor());
			// �˹� ���� ���
			FVector knockbackDir;
			knockbackDir = pfc->GetActorLocation() + (-GetActorLocation());

			float weight = (radius - knockbackDir.Length()) / 2.0f + 500.0f;
			knockbackDir.Normalize();

			if (!(pfc->GetCharacterMovement()->IsFalling()))
			{
				knockbackDir.Z += 1.0f; // ���� ���� �ƴ� ���, ĳ���͸� �پ� �ø�
			}
			pfc->GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
			pfc->GetCharacterMovement()->PendingLaunchVelocity = FVector(0.0f, 0.0f, 0.0f);
			pfc->LaunchCharacter(knockbackDir * weight, false, false);

		}
	}
	PlayBlastEffect();
	DrawDebugSphere(GetWorld(), GetActorLocation(), radius, 12, FColor::Yellow, false, 10.0f);
}

void APFProjectile::SetProjectileOwner(APFNormalCharacter* owner)
{
	projectileOwner = owner;
	GetCollisionComp()->IgnoreActorWhenMoving(projectileOwner, true);
}

void APFProjectile::ServerPlayBlastEffect_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	PlayBlastEffect();
}

void APFProjectile::PlayBlastEffect_Implementation()
{
	if (BlastParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, BlastParticle->Template, GetActorLocation(), FRotator(), FVector(8.0f, 8.0f, 8.0f));
	}
	if (BlastSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BlastSound, GetActorLocation());
	}
}