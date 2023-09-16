// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFProjectile.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;

/**
 * �߻�ü Ŭ����
 * �Ϲ� ĳ���Ͱ� ����ź ��ô �� �����Ǵ� �߻�ü�� ���
 * ��ô �� ���� �ð��� ����ϰų�, ���� ĳ���Ϳ� �浹�� ��� ����
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
	
	// ����ź �޽�
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UStaticMeshComponent* ProjectileMesh;
	// ���� ����Ʈ
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UParticleSystemComponent* BlastParticle;
	// ���� ����
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* BlastSound;

	class APFNormalCharacter* projectileOwner;
	float lifetime; // �߻�ü ����
	float maxLifetime; // �߻�ü �ִ� ���m

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

