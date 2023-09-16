// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFItem.generated.h"

/**
 * ������ Ŭ����
 * ���� �� ȸ�� �������� ���� �Ӽ� �� �Լ� ����
 */
UCLASS()
class PF_API APFItem : public AActor
{
	GENERATED_BODY()
	
public:	
	APFItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UNiagaraComponent* EffectNiagara;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UMaterial* EffectMaterial;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* AcquireSound;

protected:
	UFUNCTION()
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

public :
	// ������ ȹ������ ����Ǿ�� �ϴ� ������ Ŭ���̾�Ʈ ���α׷��� ������ �� ����
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastAcquired(AActor* actor);
	// ������ ȹ������ ����Ǿ�� �ϴ� ������ ���� ���α׷��� ������ �� ����
	UFUNCTION(Server, Reliable)
	virtual void ServerAcquired(AActor* actor);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastAcquireSound(AActor* actor);

	class USphereComponent* GetSphereComponent() const { return SphereComponent; }
	class UStaticMeshComponent* GetStaticMeshComponent() const { return Mesh; }
	class UNiagaraComponent* GetEffectNiagara() const { return EffectNiagara; }
	class UMaterial* GetEffectMaterial() const { return EffectMaterial; }
	class USoundCue* GetAcquireSound() const { return AcquireSound; }

};
