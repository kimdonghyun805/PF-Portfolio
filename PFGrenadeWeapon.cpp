// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGrenadeWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "PFProjectile.h"

UPFGrenadeWeapon::UPFGrenadeWeapon()
{
	// ���콺 �������� �߻�ü�� ����, ������ �˵��� ��������
	range = 5000.0f;
	damage = 100.0f;

	remainAmmo = 0; // ������ ��ź �� - ����
	maxRemainAmmo = 10; // �ִ�� ������ �� �ִ� ��ź ��

	loadedAmmo = 0; // ���� ������ ��ź �� - ����
	maxLoadedAmmo = 0; // �ִ�� ������ �� �ִ� ��ź ��

	bIsReloading = false; // ���� ���� ���� - Grenade������ ������� ����
	remainReloadingTime = 0.0f; // ���� �ð� - Grenade������ ������� ����
	maxReloadingTime = 0.0f; // �ִ� ���� �ð� - Grenade������ ������� ����

	maxFireDelay = 1.5f; // �ִ� �߻� ������
	fireDelay = 0.0f; // �߻� ������ - ����

	// �޽� ����
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

	// ���� ��ü ������ ���� ��ġ ����
	meshLocation1P = FVector(40.0f, 30.0f, -15.0f) - swapMovement;
	Mesh1P->SetRelativeRotation(FRotator(0.0f, -75.0f, 0.0f));
	Mesh1P->SetRelativeLocation(meshLocation1P);
	Mesh1P->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));

	//GetMesh3P()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
	//GetMesh3P()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	// ����ź ���̷�Ż �޽� ����
	//SkeletalMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade_Skeleton.SM_grenade_Skeleton'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
			TEXT("SkeletalMesh'/Game/PF/PFWeapon/Grenade/Mesh/SM_grenade_Skeleton.SM_grenade_Skeleton'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		Mesh3P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		Mesh1P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
	}
	// ����ź ���� ��Ƽ���� ����
	//MaterialInstanceConstant'/Game/PF/PFWeapon/Grenade/Materials/MI_Weapon_Grenade.MI_Weapon_Grenade'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFWeapon/Grenade/Materials/MI_Weapon_Grenade.MI_Weapon_Grenade'"));
	if (materialObj.Succeeded())
	{
		dynamicMaterial3P = Mesh3P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
		dynamicMaterial1P = Mesh1P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
	}

	// �߻�ü�� ������ ��ġ�� �߻�� ������ ������ �� ������Ʈ ����
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
		// ����ź ��ô ��, ��ô�� ǥ���ϱ� ���� �޽ð� ������ٰ� �ٽ� ǥ�õǴ� ����
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
			// ��ź ����
			--remainAmmo;
			// �߻� ������ ����
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
	// ����ź���� ������� ����
	//UE_LOG(LogTemp, Warning, TEXT("PFGrenadeWeapon Reload"));
	//Super::Reload();

}

void UPFGrenadeWeapon::LoadAmmo()
{
	// ����ź������ ������� ����
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
		// �߻�ü ����
		FActorSpawnParameters actorSpawnParams;
		actorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APFProjectile* proj = GetWorld()->SpawnActor<APFProjectile>(projClass, spawnLoc, spawnRot, actorSpawnParams);
		if (proj != nullptr)
		{
			proj->SetProjectileOwner(weaponOwner);
		}
	}
	
}
