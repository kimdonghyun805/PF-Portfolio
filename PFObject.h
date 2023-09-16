// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFObject.generated.h"

/**
 * 오브젝트 클래스
 * 각종 오브젝트의 공통 속성 및 함수 설정
 */
UCLASS()
class PF_API APFObject : public AActor
{
	GENERATED_BODY()
	
public:	
	APFObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = Gameplay)
	class USoundCue* InteractionSound;

	TArray<class AActor*> OverlappingActors;

protected:
	UFUNCTION()
	virtual void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

public :
	bool IsOverlapped() const { return OverlappingActors.Num() != 0; }

	class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	class UStaticMeshComponent* GetStaticMeshComponent() const { return Mesh; }
	class USoundCue* GetInteractionSound() const { return InteractionSound; }
};
