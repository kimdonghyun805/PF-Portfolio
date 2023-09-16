// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGrenadeWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "PFProjectile.h"

UPFGrenadeWeapon::UPFGrenadeWeapon()
{
	// 마우스 방향으로 발사체를 생성, 포물선 궤도로 날려보냄
	range = 5000.0f;
	damage = 100.0f;

	remainAmmo = 0; // 보유한 장탄 수 - 변동
	maxRemainAmmo = 10; // 최대로 보유할 수 있는 장탄 수

	loadedAmmo = 0; // 장전 상태의 장탄 수 - 변동
	maxLoadedAmmo = 0; // 최대로 장전할 수 있는 장탄 수

	bIsReloading = false; // 장전 중인 상태 - Grenade에서는 사용하지 않음
	remainReloadingTime = 0.0f; // 장전 시간 - Grenade에서는 사용하지 않음
	maxReloadingTime = 0.0f; // 최대 장전 시간 - Grenade에서는 사용하지 않음

	maxFireDelay = 1.5f; // 최대 발사 딜레이
	fireDelay = 0.0f; // 발사 딜레이 - 변동

	// 메시 설정
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GrenadeWeaponMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetVisibility(false);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GrenadeWeaponMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetVisibility(false);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 무장 교체 연출을 위해 위치 조정
	meshLocation1P = FVector(40.0f, 30.0f, -15.0f) - swapMovement;
	Mesh1P->SetRelativeRotation(FRotator(0.0f, -75.0f, 0.0f));
	Mesh1P->SetRelativeLocation(meshLocation1P);
	Mesh1P->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));

	//GetMesh3P()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
	//GetMesh3P()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	// 수류탄 스켈레탈 메시 설정
	//SkeletalMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade_Skeleton.SM_grenade_Skeleton'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
			TEXT("SkeletalMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade_Skeleton.SM_grenade_Skeleton'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		Mesh3P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		Mesh1P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
	}
	// 수류탄 색상 머티리얼 설정
	//MaterialInstanceConstant'/Game/PF/PFWeapon/Grenade/Materials/MI_Weapon_Grenade.MI_Weapon_Grenade'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFWeapon/Grenade/Materials/MI_Weapon_Grenade.MI_Weapon_Grenade'"));
	if (materialObj.Succeeded())
	{
		dynamicMaterial3P = Mesh3P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
		dynamicMaterial1P = Mesh1P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
	}

	// 발사체를 생성할 위치와 발사될 방향을 가지는 씬 컴포넌트 설정
	projSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawn"));
	projSpawn->SetupAttachment(GetMesh1P());
	projSpawn->SetRelativeRotation(FRotator(20.0f, 77.0f, 0.0f));
	projSpawn->SetRelativeLocation(FVector(10.0f, 0.0f, 5.0f));

	projClass = APFProjectile::StaticClass();
}

void UPFGrenadeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPFGrenadeWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (weaponOwner->GetLocalRole() == ROLE_AutonomousProxy && bIsPicked)
	{
		// 수류탄 투척 시, 투척을 표현하기 위해 메시가 사라졌다가 다시 표시되는 연출
		if (fireDelay <= 0.0f && !(GetMesh1P()->IsVisible()))
		{
			GetMesh1P()->SetVisibility(true);
		}
	}
}

void UPFGrenadeWeapon::OnFire(const FVector& mouseDir, const FVector& mousePos)
{
	if (fireDelay <= 0.0f && swapDelay <= 0.0f)
	{
		if (remainAmmo > 0)
		{
			ServerOnFire(projSpawn->GetComponentLocation(), projSpawn->GetComponentRotation());
			GetMesh1P()->SetVisibility(false);
			// 장탄 조정
			--remainAmmo;
			// 발사 딜레이 조정
			fireDelay = maxFireDelay;
		}
	}
}

void UPFGrenadeWeapon::AfterFire(const FVector& mouseDir, const FVector& mousePos)
{
	if (IsAmmoEmpty())
	{
		weaponOwner->SelectNextWeapon(1.0f);
	}
}

void UPFGrenadeWeapon::Reload()
{
	// 수류탄에서 사용하지 않음
	//UE_LOG(LogTemp, Warning, TEXT("PFGrenadeWeapon Reload"));
	//Super::Reload();

}

void UPFGrenadeWeapon::LoadAmmo()
{
	// 수류탄에서는 사용하지 않음
	//UE_LOG(LogTemp, Warning, TEXT("PFGrenadeWeapon LoadAmmo"));
	//Super::LoadAmmo();

}

void UPFGrenadeWeapon::ServerOnFire_Implementation(const FVector& spawnLoc, const FRotator& spawnRot)
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	Fire(spawnLoc, spawnRot);
}
bool UPFGrenadeWeapon::ServerOnFire_Validate(const FVector& spawnLoc, const FRotator& spawnRot)
{
	return true;
}

void UPFGrenadeWeapon::Fire(const FVector& spawnLoc, const FRotator& spawnRot)
{
	if (projClass != nullptr)
	{
		// 발사체 생성
		FActorSpawnParameters actorSpawnParams;
		actorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APFProjectile* proj = GetWorld()->SpawnActor<APFProjectile>(projClass, spawnLoc, spawnRot, actorSpawnParams);
		if (proj != nullptr)
		{
			proj->SetProjectileOwner(weaponOwner);
		}
	}
	
}
