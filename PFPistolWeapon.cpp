// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPistolWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UPFPistolWeapon::UPFPistolWeapon()
{
	// 가장 기본적인 무장, 기본 공격 키 입력 시 발사
	range = 3000.0f;
	damage = 15.0f;

	remainAmmo = 0; // 보유한 장탄 수 - 변동
	maxRemainAmmo = 150; // 최대로 보유할 수 있는 장탄 수

	loadedAmmo = 0; // 장전 상태의 장탄 수 - 변동
	maxLoadedAmmo = 15; // 최대로 장전할 수 있는 장탄 수

	bIsReloading = false; // 장전 중인 상태 - 변동
	remainReloadingTime = 0.0f; // 장전 시간 - 변동
	maxReloadingTime = 1.5f; // 최대 장전 시간

	maxFireDelay = 0.3f; // 최대 발사 딜레이
	fireDelay = 0.0f; // 발사 딜레이 - 변동

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

	// 권총 스켈레탈 메시 설정
	// SkeletalMesh'/Game/PF/PFWeapon/Pistol/Mesh/SK_Pistol.SK_Pistol'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
			TEXT("SkeletalMesh'/Game/PF/PFWeapon/Pistol/Mesh/SK_Pistol.SK_Pistol'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		Mesh3P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		Mesh1P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
	}
	// 권총 색상 머티리얼 설정
	//MaterialInstanceConstant'/Game/PF/PFWeapon/Pistol/Materials/MI_Weapon_Pistol.MI_Weapon_Pistol'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFWeapon/Pistol/Materials/MI_Weapon_Pistol.MI_Weapon_Pistol'"));
	if (materialObj.Succeeded())
	{
		dynamicMaterial3P = Mesh3P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
		dynamicMaterial1P = Mesh1P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
	}
	// 권총 애니메이션 설정
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
	// 권총 발사 이펙트 설정
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

				// 장탄 조정
				--loadedAmmo;
				// 발사 딜레이 조정
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
	// 장탄을 모두 소모한 경우, 무장을 자동으로 교체
	if (IsAmmoEmpty())
	{
		weaponOwner->SelectNextWeapon(1.0f);
	}
}

void UPFPistolWeapon::Reload()
{
	// 재장전 수행
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
	// 장탄 공급
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
	// 레이 트레이스 수행
	FVector start = mousePos + (mouseDir * 50.0f);
	FVector end = mousePos + (mouseDir * pistolRange);
	AActor* target = LineTrace(start, end);
	if (target != nullptr)
	{
		// 충돌한 액터에게 피해
		APFBossCharacter* bc = Cast<APFBossCharacter>(target);
		FDamageEvent pistolDamageEvent(UDamageType::StaticClass());
		// 일정 량의 피해
		bc->TakeDamage(pistolDamage, pistolDamageEvent, weaponOwner->GetController(), weaponOwner);
	}
	
}


