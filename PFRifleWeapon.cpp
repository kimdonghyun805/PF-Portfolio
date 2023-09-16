// Fill out your copyright notice in the Description page of Project Settings.

#include "PFRifleWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"

UPFRifleWeapon::UPFRifleWeapon()
{
	// �⺻ ���� Ű �Է��� ������ ��(OnFire)�� ������ �ڵ����� �߻�
	// �ڵ����� �߻��� ź���� ���� numRecoilAmmo ���� ū ���, �߻� ������ ������ ȸ��
	range = 6000.0f;
	damage = 20.0f;

	remainAmmo = 0; // ������ ��ź �� - ����
	maxRemainAmmo = 240; // �ִ�� ������ �� �ִ� ��ź ��

	loadedAmmo = 0; // ���� ������ ��ź �� - ����
	maxLoadedAmmo = 30; // �ִ�� ������ �� �ִ� ��ź ��

	bIsReloading = false; // ���� ���� ���� - ����
	remainReloadingTime = 0.0f; // ���� �ð� - ����
	maxReloadingTime = 1.5f; // �ִ� ���� �ð�

	maxFireDelay = 0.1f; // �ִ� �߻� ������
	fireDelay = 0.0f; // �߻� ������ - ����

	bIsAutoFiring = false;
	numAutoFiredAmmo = 0.0f;
	numRecoilAmmo = 4; // 6��° ���� ���� �ݵ� ����

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleWeaponMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetVisibility(false);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleWeaponMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetVisibility(false);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	meshLocation1P = FVector(40.0f, 30.0f, -15.0f) - swapMovement;
	Mesh1P->SetRelativeRotation(FRotator(0.0f, -75.0f, 0.0f));
	Mesh1P->SetRelativeLocation(meshLocation1P);
	Mesh1P->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));

	//GetMesh3P()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//GetMesh3P()->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	//GetMesh3P()->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	// ���� ���̷�Ż �޽� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
			TEXT("SkeletalMesh'/Game/PF/PFWeapon/Rifle/Mesh/SK_Rifle.SK_Rifle'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		Mesh3P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		Mesh1P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
	}
	// ���� ���� ��Ƽ���� ����
	//MaterialInstanceConstant'/Game/PF/PFWeapon/Rifle/Materials/MI_Weapon_Rifle.MI_Weapon_Rifle'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFWeapon/Rifle/Materials/MI_Weapon_Rifle.MI_Weapon_Rifle'"));
	if (materialObj.Succeeded())
	{
		dynamicMaterial3P = Mesh3P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
		dynamicMaterial1P = Mesh1P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
	}
	
	// ���� �ִϸ��̼� ����
	//AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_MM_Rifle_Fire.AM_MM_Rifle_Fire'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> fireAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_MM_Rifle_Fire.AM_MM_Rifle_Fire'"));
	if (fireAnimObj.Succeeded())
	{
		CharacterFireAnimMontage = fireAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_MM_Rifle_Reload.AM_MM_Rifle_Reload'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> reloadAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_MM_Rifle_Reload.AM_MM_Rifle_Reload'"));
	if (reloadAnimObj.Succeeded())
	{
		CharacterReloadAnimMontage = reloadAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_MM_Rifle_Equip.AM_MM_Rifle_Equip'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> equipAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_MM_Rifle_Equip.AM_MM_Rifle_Equip'"));
	if (equipAnimObj.Succeeded())
	{
		CharacterEquipAnimMontage = equipAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_Weap_Rifle_Fire.AM_Weap_Rifle_Fire'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> weaponFireAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_Weap_Rifle_Fire.AM_Weap_Rifle_Fire'"));
	if (weaponFireAnimObj.Succeeded())
	{
		WeaponFireAnimMontage = weaponFireAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_Weap_Rifle_Reload.AM_Weap_Rifle_Reload'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> weaponReloadAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Rifle/Animations/AM_Weap_Rifle_Reload.AM_Weap_Rifle_Reload'"));
	if (weaponReloadAnimObj.Succeeded())
	{
		WeaponReloadAnimMontage = weaponReloadAnimObj.Object;
	}
	// ���� �߻� ����Ʈ ����
	FireParticle1P = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle1P"));
	FireParticle1P->bAutoActivate = false;
	FireParticle1P->SetOnlyOwnerSee(true);
	FireParticle1P->SetRelativeScale3D(FVector(0.06f, 0.06f, 0.06f));
	FireParticle3P = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle3P"));
	FireParticle3P->bAutoActivate = false;
	FireParticle3P->SetOwnerNoSee(true);
	FireParticle3P->SetRelativeScale3D(FVector(0.08f, 0.08f, 0.08f));
	//ParticleSystem'/Game/PF/Effect/Particle/P_Explosion.P_Explosion'
	static ConstructorHelpers::FObjectFinder<UParticleSystem> fireParticleObj(
		TEXT("ParticleSystem'/Game/PF/Effect/Particle/P_Explosion.P_Explosion'"));
	if (fireParticleObj.Succeeded())
	{
		FireParticle1P->SetTemplate(fireParticleObj.Object);
		FireParticle1P->SetupAttachment(GetMesh1P(), TEXT("Muzzle"));
		FireParticle3P->SetTemplate(fireParticleObj.Object);
		FireParticle3P->SetupAttachment(GetMesh3P(), TEXT("Muzzle"));
	}
}

void UPFRifleWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPFRifleWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (weaponOwner->GetLocalRole() == ROLE_AutonomousProxy && bIsPicked)
	{
		if (bIsAutoFiring && fireDelay <= 0.0f)
		{
			TArray<FVector> mouseDirAndPos = weaponOwner->GetMouseDirectionAndPosition();
			FVector mouseDir = mouseDirAndPos[0];
			FVector mousePos = mouseDirAndPos[1];
			if (numAutoFiredAmmo > numRecoilAmmo)
			{
				// ���� ���⿡ �ݵ� ����
				FVector rotAxis = FVector::VectorPlaneProject(weaponOwner->GetActorForwardVector(), mouseDir);
				rotAxis.Normalize();
				float randDegree = FMath::RandRange(-180.0f, 180.0f);
				rotAxis = rotAxis.RotateAngleAxis(randDegree, mouseDir);
				float randAngle = FMath::RandRange(0.5f, 2.0f);
				mouseDir = mouseDirAndPos[0].RotateAngleAxis(randAngle, rotAxis);
			}
			OnFire(mouseDir, mousePos);
			++numAutoFiredAmmo;
		}
	}
}

void UPFRifleWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UPFRifleWeapon, bIsAutoFiring);
}

void UPFRifleWeapon::OnFire(const FVector& mouseDir, const FVector& mousePos)
{
	if (swapDelay <= 0.0f)
	{
		if (!bIsReloading && fireDelay <= 0.0f)
		{
			if (loadedAmmo > 0)
			{
				ServerOnFire(mouseDir, mousePos, damage, range);
				ServerPlayFireAnim();
				if (!bIsAutoFiring)
				{
					bIsAutoFiring = true;
				}

				// ��ź ����
				--loadedAmmo;
				// �߻� ������ ����
				fireDelay = maxFireDelay;
			}
			else
			{
				Reload();
			}

		}
	}
	
}

void UPFRifleWeapon::AfterFire(const FVector& mouseDir, const FVector& mousePos)
{
	bIsAutoFiring = false;
	numAutoFiredAmmo = 0;

	if (IsAmmoEmpty())
	{
		weaponOwner->SelectNextWeapon(1.0f);
	}
}

void UPFRifleWeapon::Reload()
{
	// ������ ����
	if (!bIsReloading)
	{
		if (loadedAmmo < maxLoadedAmmo && remainAmmo > 0)
		{
			if (bIsAutoFiring)
			{
				bIsAutoFiring = false;
				numAutoFiredAmmo = 0;
			}
			bIsReloading = true;
			remainReloadingTime = maxReloadingTime;
			ServerPlayReloadAnim();
		}
	}
}

void UPFRifleWeapon::LoadAmmo()
{
	// ��ź ����
	uint8 ammo = maxLoadedAmmo - loadedAmmo;
	if (ammo >= remainAmmo)
	{
		loadedAmmo += remainAmmo;
		remainAmmo = 0;
	}
	else
	{
		loadedAmmo += ammo;
		remainAmmo -= ammo;
	}
	bIsReloading = false;
}

void UPFRifleWeapon::UnpickThisWeapon()
{
	Super::UnpickThisWeapon();
	bIsAutoFiring = false;
	numAutoFiredAmmo = 0;
	GetMesh1P()->SetRelativeLocation(meshLocation1P);
}

void UPFRifleWeapon::ServerOnFire_Implementation(const FVector& mouseDir, const FVector& mousePos, const float& rifleDamage, const float& rifleRange)
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	Fire(mouseDir, mousePos, rifleDamage, rifleRange);
}

bool UPFRifleWeapon::ServerOnFire_Validate(const FVector& mouseDir, const FVector& mousePos, const float& rifleDamage, const float& rifleRange)
{
	if (rifleDamage < 0.0f || rifleRange < 0.0f)
	{
		return false;
	}
	return true;
}

void UPFRifleWeapon::Fire(const FVector& mouseDir, const FVector& mousePos, const float& rifleDamage, const float& rifleRange)
{
	// ���� Ʈ���̽� ����
	FVector start = mousePos + (mouseDir * 50.0f);
	FVector end = mousePos + (mouseDir * rifleRange);
	AActor* target = LineTrace(start, end);
	if (target != nullptr)
	{
		// �浹�� ���Ϳ��� ����
		APFBossCharacter* bc = Cast<APFBossCharacter>(target);
		FDamageEvent rifleDamageEvent(UDamageType::StaticClass());
		// ���� ���� ����
		bc->TakeDamage(rifleDamage, rifleDamageEvent, weaponOwner->GetController(), weaponOwner);
	}
}


