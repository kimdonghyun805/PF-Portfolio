// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFObject.h"
#include "PFCoreObject.generated.h"

/**
 * 코어 오브젝트 클래스
 * 보스 캐릭터에 의해 피해를 입고 파괴될 수 있음
 * 코어 오브젝트가 파괴되면, 보스 캐릭터를 강화하고 구역을 붕괴
 */
UCLASS()
class PF_API APFCoreObject : public APFObject
{
	GENERATED_BODY()
		
public :
	APFCoreObject();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventIntigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UStaticMeshComponent* CoreMeshCenter;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UStaticMeshComponent* CoreMeshTop;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UStaticMeshComponent* CoreMeshBottom;

	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UParticleSystemComponent* DestructionParticle;

	UPROPERTY(VisibleAnywhere, Category = CoreObject)
	class APFCollapseDistrict* OwningDistrict;

	UPROPERTY(Replicated)
	float health;
	const float maxHealth = 500.0f;

	UFUNCTION(Netmulticast, Reliable)
	void PlayDestructionEffect();

public :
	UStaticMeshComponent* GetCoreMeshCenter() const { return CoreMeshCenter; }
	UStaticMeshComponent* GetCoreMeshTop() const { return CoreMeshTop; }
	UStaticMeshComponent* GetCoreMeshBottom() const { return CoreMeshBottom; }
	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return health; }
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return maxHealth; }

	void SetOwningDistrict(class APFCollapseDistrict* district);
};
