// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

/**
 * 캐릭터 클래스
 * 보스 캐릭터 및 일반 캐릭터의 공통 속성 및 함수를 설정
 */
UCLASS(config=Game)
class PF_API APFCharacter : public ACharacter
{
	GENERATED_BODY()
		
public:
	APFCharacter();

protected:
	virtual void BeginPlay();

	virtual void OnPrimaryAction();
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LookUp(float value);
	void LookRight(float value);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void Tick(float DeltaTime) override;

public :
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventIntigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(Replicated)
	float aim;
	UPROPERTY(Replicated)
	float moveRight;
	UPROPERTY(Replicated)
	float moveForward;

public :
	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


	UFUNCTION(Server, Unreliable)
	void ServerSetAim(); // 캐릭터가 바라보는 방향 계산
	UFUNCTION(Server, Unreliable)
	void ServerSetMovement(); // 캐릭터의 이동 방향 계산

	UFUNCTION(Netmulticast, Reliable)
	void PlayRagdoll(); // 캐릭터를 래그돌로 변환

	UFUNCTION(BlueprintCallable)
	float GetAim() const { return aim; }
	UFUNCTION(BlueprintCallable)
	TArray<float> GetMovement() const;

};

