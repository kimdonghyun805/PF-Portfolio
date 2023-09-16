// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFDistrict.generated.h"

/**
 * 구역 클래스
 * 중앙 구역 및 붕괴 구역의 공통 속성 및 함수를 설정
 */
UCLASS()
class PF_API APFDistrict : public AActor
{
	GENERATED_BODY()
	
public:	
	APFDistrict();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(EditAnywhere, Category = District)
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere, Category = District)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = District)
	class APFObserverCamera* ObserverCamera;

	TArray<class APFNormalCharacter*> OverlappingNormals;
	TArray<class APFBossCharacter*> OverlappingBosses;

protected:
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

public :
	TArray<class APFNormalCharacter*> GetOverlappingNormals() const { return OverlappingNormals; }
	TArray<class APFBossCharacter*> GetOverlappingBosses() const { return OverlappingBosses; }
	class UBoxComponent* GetBoxComponent() const { return BoxComponent; }
	class UStaticMeshComponent* GetMesh() const { return Mesh; }
	class APFObserverCamera* GetObserverCamera() const { return ObserverCamera; }
};
