// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFItem.generated.h"

/**
 * 아이템 클래스
 * 무장 및 회복 아이템의 공통 속성 및 함수 설정
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
	// 아이템 획득으로 변경되어야 하는 변수가 클라이언트 프로그램에 존재할 때 실행
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastAcquired(AActor* actor);
	// 아이템 획득으로 변경되어야 하는 변수가 서버 프로그램에 존재할 때 실행
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
