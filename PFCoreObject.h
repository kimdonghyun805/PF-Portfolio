// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFObject.h"
#include "PFCoreObject.generated.h"

/**
 * �ھ� ������Ʈ Ŭ����
 * ���� ĳ���Ϳ� ���� ���ظ� �԰� �ı��� �� ����
 * �ھ� ������Ʈ�� �ı��Ǹ�, ���� ĳ���͸� ��ȭ�ϰ� ������ �ر�
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
