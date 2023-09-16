// Copyright Epic Games, Inc. All Rights Reserved.

#include "PFCharacter.h"
#include "PFProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraActor.h"


APFCharacter::APFCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	RootComponent = GetCapsuleComponent();

	// 체공 시 이동속도 조정
	GetCharacterMovement()->AirControl = 0.5f;

	// 플레이어용 1인칭 카메라 컴포넌트
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	// 1인칭 플레이어 전용 메시
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Mesh1P->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//Mesh1P->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	//Mesh1P->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	// 3인칭 메시
	Mesh3P = GetMesh();
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void APFCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

void APFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetController() != nullptr && GetPlayerState() != nullptr)
	{
		if (GetVelocity().IsNearlyZero() && (moveRight != 0.0f || moveForward != 0.0f))
		{
			// 캐릭터가 움직일 때 마다 이동 방향을 계산
			ServerSetMovement();
		}
	}
}

void APFCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APFCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APFCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APFCharacter::LookRight);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APFCharacter::LookUp);
}
float APFCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
	AController* EventIntigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventIntigator, DamageCauser);
	return Damage;
}

void APFCharacter::OnPrimaryAction()
{
	// 기본 공격 키 입력 시 실행
}

void APFCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
		// 캐릭터가 움직일 때 마다 이동 방향을 계산
		if (GetController() != nullptr && GetPlayerState() != nullptr)
		{
			ServerSetMovement();
		}
	}
}

void APFCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
		// 캐릭터가 움직일 때 마다 이동 방향을 계산
		if (GetController() != nullptr && GetPlayerState() != nullptr)
		{
			ServerSetMovement();
		}
	}
}

void APFCharacter::LookUp(float value)
{
	if (value != 0.0f)
	{
		AddControllerPitchInput(value);
		// 캐릭터 시점이 회전할 때 마다 시점 각도를 계산
		if (GetController() != nullptr && GetPlayerState() != nullptr)
		{
			ServerSetAim();
		}
	}
}

void APFCharacter::LookRight(float value)
{
	if (value != 0.0f)
	{
		AddControllerYawInput(value);
		if (GetController() != nullptr && GetPlayerState() != nullptr)
		{
			// 캐릭터 시점이 회전할 때 마다 시점 각도를 계산
			ServerSetAim();
		}
	}
}

void APFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFCharacter, aim);
	DOREPLIFETIME(APFCharacter, moveRight);
	DOREPLIFETIME(APFCharacter, moveForward);
}

void APFCharacter::PlayRagdoll_Implementation()
{
	// 캐릭터를 래그돌로 변환
	GetMesh3P()->SetPhysicsBlendWeight(1.0f);
	GetMesh3P()->SetSimulatePhysics(true);
	GetMesh3P()->SetCollisionProfileName("Ragdoll");
	GetMesh3P()->SetOwnerNoSee(false);
	GetMesh3P()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void APFCharacter::ServerSetAim_Implementation()
{
	float a = 0.0f;
	if (GetController() != nullptr)
	{
		a = GetController()->GetControlRotation().Pitch;
		if (a > 180)
		{
			a -= 360.0f;
		}
	}
	aim = a;
}

void APFCharacter::ServerSetMovement_Implementation()
{
	FVector velocity = GetVelocity();
	velocity.Z = 0.0f;
	velocity.Normalize();
	FVector direction = GetActorForwardVector();
	direction.Z = 0.0f;
	direction.Normalize();

	float dot = FVector::DotProduct(velocity, direction);
	FVector cross = FVector::CrossProduct(velocity, direction);
	float acos = FMath::Acos(dot);
	if (cross.Z > 0)
	{
		acos = 2 * PI - acos;
	}

	if (velocity.IsNearlyZero())
	{
		moveForward = 0.0f;
		moveRight = 0.0f;
	}
	else
	{
		moveForward = FMath::Cos(acos) * 10;
		moveRight = FMath::Sin(acos) * 10;
	}
}

TArray<float> APFCharacter::GetMovement() const
{
	TArray<float> moveDir;
	moveDir.Insert(moveForward, 0);
	moveDir.Insert(moveRight, 1);
	return moveDir;
}
