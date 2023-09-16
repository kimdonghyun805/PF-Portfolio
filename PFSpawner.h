// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFSpawner.generated.h"

/**
 * 스포너 클래스
 * 각종 스포너의 공통 속성 및 함수 설정
 */
UCLASS()
class PF_API APFSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APFSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UStaticMeshComponent* Mesh;

	TArray<class AActor*> OverlappingActors;

	float spawnDelay;
	float maxSpawnDelay;

protected :
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

public :
	bool CanSpawn() const;

	class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	class UStaticMeshComponent* GetStaticMeshComponent() const { return Mesh; }
	bool IsOverlapped() const { return OverlappingActors.Num() != 0; }
	float GetSpawnDelay() const { return spawnDelay; }
	float GetMaxSpawnDelay() const { return maxSpawnDelay; }
	void SetMaxSpawnDelay(const float& value);
};
