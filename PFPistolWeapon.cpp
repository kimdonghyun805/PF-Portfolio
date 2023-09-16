// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPistolWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UPFPistolWeapon::UPFPistolWeapon()
{
	// ���� �⺻���� ����, �⺻ ���� Ű �Է� �� �߻�
	range = 3000.0f;
	damage = 15.0f;

	remainAmmo = 0; // ������ ��ź �� - ����
	maxRemainAmmo = 150; // �ִ�� ������ �� �ִ� ��ź ��

	loadedAmmo = 0; // ���� ������ ��ź �� - ����
	maxLoadedAmmo = 15; // �ִ�� ������ �� �ִ� ��ź ��

	bIsReloading = false; // ���� ���� ���� - ����
	remainReloadingTime = 0.0f; // ���� �ð� - ����
	maxReloadingTime = 1.5f; // �ִ� ���� �ð�

	maxFireDelay = 0.3f; // �ִ� �߻� ������
	fireDelay = 0.0f; // �߻� ������ - ����

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolWeaponMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetVisibility(false);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PistolWeaponMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetVisibility(false);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	meshLocation1P = FVector(50.0f, 40.0f, -15.0f) - swapMovement;
	Mesh1P->SetRelativeRotation(FRotator(0.0f, -80.0f, -5.0f));
	Mesh1P->SetRelativeLocation(meshLocation1P);
	Mesh1P->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));

	Mesh3P->SetRelativeRotation(FRotator(5.0f, -10.0f, 0.0f));
	//GetMesh3P()->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	//GetMesh3P()->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	// ���� ���̷�Ż �޽� ����
	// SkeletalMesh'/Game/PF/PFWeapon/Pistol/Mesh/SK_Pistol.SK_Pistol'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
			TEXT("SkeletalMesh'/Game/PF/PFWeapon/Pistol/Mesh/SK_Pistol.SK_Pistol'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		Mesh3P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		Mesh1P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
	}
	// ���� ���� ��Ƽ���� ����
	//MaterialInstanceConstant'/Game/PF/PFWeapon/Pistol/Materials/MI_Weapon_Pistol.MI_Weapon_Pistol'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFWeapon/Pistol/Materials/MI_Weapon_Pistol.MI_Weapon_Pistol'"));
	if (materialObj.Succeeded())
	{
		dynamicMaterial3P = Mesh3P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
		dynamicMaterial1P = Mesh1P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
	}
	// ���� �ִϸ��̼� ����
	//AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_MM_Pistol_Fire.AM_MM_Pistol_Fire'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> fireAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_MM_Pistol_Fire.AM_MM_Pistol_Fire'"));
	if (fireAnimObj.Succeeded())
	{
		CharacterFireAnimMontage = fireAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_MM_Pistol_Reload.AM_MM_Pistol_Reload'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> reloadAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_MM_Pistol_Reload.AM_MM_Pistol_Reload'"));
	if (reloadAnimObj.Succeeded())
	{
		CharacterReloadAnimMontage = reloadAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_MM_Pistol_Equip.AM_MM_Pistol_Equip'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> equipAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_MM_Pistol_Equip.AM_MM_Pistol_Equip'"));
	if (equipAnimObj.Succeeded())
	{
		CharacterEquipAnimMontage = equipAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_Weap_Pistol_Fire.AM_Weap_Pistol_Fire'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> weaponFireAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_Weap_Pistol_Fire.AM_Weap_Pistol_Fire'"));
	if (weaponFireAnimObj.Succeeded())
	{
		WeaponFireAnimMontage = weaponFireAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_Weap_Pistol_Reload.AM_Weap_Pistol_Reload'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> weaponReloadAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/Pistol/Animations/AM_Weap_Pistol_Reload.AM_Weap_Pistol_Reload'"));
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

void UPFPistolWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPFPistolWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UPFPistolWeapon::OnFire(const FVector& mouseDir, const FVector& mousePos)
{
	if (swapDelay <= 0.0f)
	{
		if (!bIsReloading && fireDelay <= 0.0f)
		{
			if (loadedAmmo > 0)
			{
				ServerOnFire(mouseDir, mousePos, damage, range);
				ServerPlayFireAnim();

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

void UPFPistolWeapon::AfterFire(const FVector& mouseDir, const FVector& mousePos)
{
	// ��ź�� ��� �Ҹ��� ���, ������ �ڵ����� ��ü
	if (IsAmmoEmpty())
	{
		weaponOwner->SelectNextWeapon(1.0f);
	}
}

void UPFPistolWeapon::Reload()
{
	// ������ ����
	if (!bIsReloading)
	{
		if (loadedAmmo < maxLoadedAmmo && remainAmmo > 0)
		{
			bIsReloading = true;
			remainReloadingTime = maxReloadingTime;
			ServerPlayReloadAnim();
		}
	}
	
}

void UPFPistolWeapon::LoadAmmo()
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

void UPFPistolWeapon::ServerOnFire_Implementation(const FVector& mouseDir, const FVector& mousePos, const float& pistolDamage, const float& pistolRange)
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	Fire(mouseDir, mousePos, pistolDamage, pistolRange);
}

bool UPFPistolWeapon::ServerOnFire_Validate(const FVector& mouseDir, const FVector& mousePos, const float& pistolDamage, const float& pistolRange)
{
	if (pistolDamage < 0.0f || pistolRange < 0.0f)
	{
		return false;
	}
	return true;
}

void UPFPistolWeapon::Fire(const FVector& mouseDir, const FVector& mousePos, const float& pistolDamage, const float& pistolRange)
{
	// ���� Ʈ���̽� ����
	FVector start = mousePos + (mouseDir * 50.0f);
	FVector end = mousePos + (mouseDir * pistolRange);
	AActor* target = LineTrace(start, end);
	if (target != nullptr)
	{
		// �浹�� ���Ϳ��� ����
		APFBossCharacter* bc = Cast<APFBossCharacter>(target);
		FDamageEvent pistolDamageEvent(UDamageType::StaticClass());
		// ���� ���� ����
		bc->TakeDamage(pistolDamage, pistolDamageEvent, weaponOwner->GetController(), weaponOwner);
	}
	
}


