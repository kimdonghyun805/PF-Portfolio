// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFProjectile.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;

/**
 * 발사체 클래스
 * 일반 캐릭터가 수류탄 투척 시 생성되는 발사체로 사용
 * 투척 후 일정 시간이 경과하거나, 보스 캐릭터와 충돌한 경우 폭파
 */
UCLASS(config=Game)
class APFProjectile : public AActor
{
	GENERATED_BODY()

public:
	APFProjectile();
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	// 수류탄 메시
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UStaticMeshComponent* ProjectileMesh;
	// 폭발 이펙트
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UParticleSystemComponent* BlastParticle;
	// 폭발 사운드
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* BlastSound;

	class APFNormalCharacter* projectileOwner;
	float lifetime; // 발사체 수명
	float maxLifetime; // 발사체 최대 수몀

public :
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void ServerBlast();
	UFUNCTION(Server, Reliable)
	void ServerPlayBlastEffect();
	UFUNCTION(NetMulticast, Reliable)
	void PlayBlastEffect();

	void SetProjectileOwner(class APFNormalCharacter* owner);

	UCapsuleComponent* GetCollisionComp() const { return CollisionComp; }
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
};

