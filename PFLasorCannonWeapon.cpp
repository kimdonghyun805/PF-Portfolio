// Fill out your copyright notice in the Description page of Project Settings.


#include "PFLasorCannonWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

UPFLasorCannonWeapon::UPFLasorCannonWeapon()
{
	// �⺻ ���� Ű �Է��� ������ ��(OnFire)�� 0.05�� �� 1�� loadedAmmo ����, �ִ� 50���� ����
	// �⺻ ���� Ű �Է� ���� ��(AfterFire)�� loadedAmmo * damage ��ŭ�� ������ �߻��Ͽ� ����
	range = 10000.0f;
	damage = 5.0f;

	remainAmmo = 0; // ������ ��ź �� - ����
	maxRemainAmmo = 200; // �ִ�� ������ �� �ִ� ��ź ��

	loadedAmmo = 0; // ���� ������ ��ź �� - ����
	maxLoadedAmmo = 50; // �ִ�� ������ �� �ִ� ��ź ��

	bIsReloading = false; // ���� ���� ���� - Lasor Cannon������ ������� ����
	remainReloadingTime = 0.0f; // ���� �ð� - Lasor Cannon������ ������� ����
	maxReloadingTime = 0.0f; // �ִ� ���� �ð� - Lasor Cannon������ ������� ����

	maxFireDelay = 2.0f; // �ִ� �߻� ������
	fireDelay = 0.0f; // �߻� ������ - ����

	bIsCharging = false;
	chargeTime = 0.0f;

	radius = 20.0f;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LasorCannonWeaponMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetVisibility(false);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LasorCannonWeaponMesh3P"));
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

	// ������ĳ�� ���̷�Ż �޽� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
			TEXT("SkeletalMesh'/Game/PF/PFWeapon/LasorCannon/Mesh/SKM_Shotgun.SKM_Shotgun'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		Mesh3P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		Mesh1P->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
	}
	// ������ĳ�� ���� ��Ƽ���� ����
	//MaterialInstanceConstant'/Game/PF/PFWeapon/LasorCannon/Materials/MI_Weapon_Shotgun.MI_Weapon_Shotgun'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFWeapon/LasorCannon/Materials/MI_Weapon_Shotgun.MI_Weapon_Shotgun'"));
	if (materialObj.Succeeded())
	{
		dynamicMaterial3P = Mesh3P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
		dynamicMaterial1P = Mesh1P->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialObj.Object);
	}
	// ������ĳ�� �ִϸ��̼� ����
	//AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_MM_Shotgun_Fire.AM_MM_Shotgun_Fire'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> fireAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_MM_Shotgun_Fire.AM_MM_Shotgun_Fire'"));
	if (fireAnimObj.Succeeded())
	{
		CharacterFireAnimMontage = fireAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_MM_Shotgun_Reload.AM_MM_Shotgun_Reload'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> reloadAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_MM_Shotgun_Reload.AM_MM_Shotgun_Reload'"));
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
	//AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_Weap_Shotgun_Fire.AM_Weap_Shotgun_Fire'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> weaponFireAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_Weap_Shotgun_Fire.AM_Weap_Shotgun_Fire'"));
	if (weaponFireAnimObj.Succeeded())
	{
		WeaponFireAnimMontage = weaponFireAnimObj.Object;
	}
	//AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_Weap_Shotgun_Reload.AM_Weap_Shotgun_Reload'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> weaponReloadAnimObj(
		TEXT("AnimMontage'/Game/PF/PFWeapon/LasorCannon/Animations/AM_Weap_Shotgun_Reload.AM_Weap_Shotgun_Reload'"));
	if (weaponReloadAnimObj.Succeeded())
	{
		WeaponReloadAnimMontage = weaponReloadAnimObj.Object;
	}
	// ������ ĳ�� ���� ����Ʈ ����
	ChargeParticle1P = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChargeNiagara1P"));
	ChargeParticle1P->bAutoActivate = false;
	ChargeParticle1P->SetOnlyOwnerSee(true);
	ChargeParticle1P->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
	//ChargeParticle1P->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	ChargeParticle1P->SetRelativeLocation(FVector(0.0f, 0.0f, -22.0f));
	ChargeParticle3P = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChargeNiagara3P"));
	ChargeParticle3P->bAutoActivate = false;
	ChargeParticle3P->SetOwnerNoSee(true);
	ChargeParticle3P->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));
	//ChargeParticle1P->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	ChargeParticle3P->SetRelativeLocation(FVector(0.0f, 0.0f, -25.0f));
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/LasorCannonCharge.LasorCannonCharge'
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> chargeNiagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/LasorCannonCharge.LasorCannonCharge'"));
	if (chargeNiagaraObj.Succeeded())
	{
		ChargeParticle1P->SetAsset(chargeNiagaraObj.Object);
		ChargeParticle1P->SetupAttachment(GetMesh1P(), TEXT("Magazine"));
		ChargeParticle3P->SetAsset(chargeNiagaraObj.Object);
		ChargeParticle3P->SetupAttachment(GetMesh3P(), TEXT("Magazine"));
	}
	// ������ ĳ�� �߻� ����Ʈ ����
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Lasor.Lasor'
	LasorNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LasorNiagara"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> lasorNiagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Lasor.Lasor'"));
	if (lasorNiagaraObj.Succeeded())
	{
		LasorNiagara->SetAsset(lasorNiagaraObj.Object);
		LasorNiagara->bAutoActivate = false;
	}
	else
	{
		LasorNiagara = nullptr;
	}
	// ������ ĳ�� �߻� ���� ����
	//SoundCue'/Game/PF/Audio/Weapon/LasorCannonFire.LasorCannonFire'
	static ConstructorHelpers::FObjectFinder<USoundCue> lasorSoundObj(
		TEXT("SoundCue'/Game/PF/Audio/Weapon/LasorCannonFire.LasorCannonFire'"));
	if (lasorSoundObj.Succeeded())
	{
		LasorSound = lasorSoundObj.Object;
	}
	else
	{
		LasorSound = nullptr;
	}
}

void UPFLasorCannonWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void UPFLasorCannonWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (weaponOwner->GetLocalRole() == ROLE_AutonomousProxy && bIsPicked)
	{
		if (bIsCharging && remainAmmo > 0)
		{
			// �ִ� 2.5�� ���� 50 ��ŭ ����
			if (loadedAmmo < maxLoadedAmmo)
			{
				if (chargeTime / 0.05 > 1.0f)
				{
					++loadedAmmo;
					--remainAmmo;
					chargeTime = 0.0f;
				}
				chargeTime += DeltaTime;
			}
		}
	}
}

void UPFLasorCannonWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UPFLasorCannonWeapon, bIsCharging);
}

void UPFLasorCannonWeapon::OnFire(const FVector& mouseDir, const FVector& mousePos)
{
	if (fireDelay <= 0.0f && swapDelay <= 0.0f)
	{
		if (remainAmmo > 0)
		{
			ServerPlayChargeEffect(true);
			ServerPlayFireAnim();
			chargeTime = 0.0f;
			bIsCharging = true;
		}
	}
	
}

void UPFLasorCannonWeapon::AfterFire(const FVector& mouseDir, const FVector& mousePos)
{
	if (bIsCharging && loadedAmmo > 0)
	{
		ServerAfterFire(mouseDir, mousePos, damage, range, radius, loadedAmmo);
		ServerPlayReloadAnim();

		// ��ź ����
		loadedAmmo = 0;
		// �߻� ������ ����
		fireDelay = maxFireDelay;
	}
	ServerPlayChargeEffect(false);
	bIsCharging = false;
	chargeTime = 0.0f;

	if (IsAmmoEmpty())
	{
		weaponOwner->SelectNextWeapon(1.0f);
	}
}

void UPFLasorCannonWeapon::Reload()
{
	//UE_LOG(LogTemp, Warning, TEXT("PFLasorCannonWeapon Reload"));
	//Super::Reload();
	// ������ ���� ��Ŀ���� ���, �ش� �Լ��� �������� ����

}

void UPFLasorCannonWeapon::LoadAmmo()
{
	//UE_LOG(LogTemp, Warning, TEXT("PFLasorCannonWeapon LoadAmmo"));
	//Super::LoadAmmo();
	// ������ ���� ��Ŀ���� ���, �ش� �Լ��� �������� ����
}

void UPFLasorCannonWeapon::UnpickThisWeapon()
{
	Super::UnpickThisWeapon();
	if (bIsCharging)
	{
		remainAmmo += loadedAmmo;
		loadedAmmo = 0;
	}
	bIsCharging = false;
	chargeTime = 0.0f;
	ServerPlayChargeEffect(false);

	GetMesh1P()->SetRelativeLocation(meshLocation1P);
}

void UPFLasorCannonWeapon::ServerAfterFire_Implementation(const FVector& mouseDir, const FVector& mousePos, const float& lasorDamage, 
	const float& lasorRange, const float& lasorRadius, const uint8& ammo)
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	Fire(mouseDir, mousePos, lasorDamage, lasorRange, lasorRadius, ammo);

}

bool UPFLasorCannonWeapon::ServerAfterFire_Validate(const FVector& mouseDir, const FVector& mousePos, const float& lasorDamage, 
	const float& lasorRange, const float& lasorRadius, const uint8& ammo)
{
	return true;
}

void UPFLasorCannonWeapon::Fire(const FVector& mouseDir, const FVector& mousePos, const float& lasorDamage, 
	const float& lasorRange, const float& lasorRadius, const uint8& ammo)
{
	FVector start = mousePos + (mouseDir * 120.0f);
	FVector end = mousePos + (mouseDir * lasorRange);
	// �������� ���� ������ ���� ����
	float thisRadius, thisDamage;
	if (ammo > 20.0f)
	{
		thisRadius = (float)ammo * 2.0f;
		thisDamage = lasorDamage + (ammo / 5);
	}
	else
	{
		thisRadius = lasorRadius;
		thisDamage = lasorDamage;
	}
	uint8 numAttackedObject = 0;
	// ��ü�� sweep ����Ͽ� ���� ������ ���� ĳ���� Ž��
	FCollisionObjectQueryParams targetObj;
	FCollisionQueryParams thisObj(FName(TEXT("LasorCannonSweep")), true, weaponOwner);
	// sweep multi
	targetObj.AddObjectTypesToQuery(BossCollisionChannel);
	TArray<FHitResult> hitResult;
	GetWorld()->SweepMultiByObjectType(hitResult, start, end, FQuat(), targetObj, FCollisionShape::MakeSphere(thisRadius), thisObj);
	for (auto& hit : hitResult)
	{
		if (hit.GetActor()->GetClass()->IsChildOf<APFBossCharacter>()) // ������ ������Ʈ�� Ÿ�� ������� Ȯ��
		{
			// �浹�� ���Ϳ��� ����
			APFBossCharacter* bc = Cast<APFBossCharacter>(hit.GetActor());
			FDamageEvent lasorCannonDamageEvent(UDamageType::StaticClass());
			// ���� ���� ����
			bc->TakeDamage((thisDamage * (float)ammo), lasorCannonDamageEvent, weaponOwner->GetController(), weaponOwner);
			++numAttackedObject;
		}
	}
	PlayLasorEffect(start, end, thisRadius);
	// ����� ��ü �׸���
	/*FColor col = FColor::Yellow;
	if (numAttackedObject > 0) { col = FColor::Green; }
	else { col = FColor::Red; }
	DrawDebugSphere(GetWorld(), start, thisRadius, 12, col, false, 10.0f);
	DrawDebugSphere(GetWorld(), end, thisRadius, 12, col, false, 10.0f);*/
}

void UPFLasorCannonWeapon::ServerPlayChargeEffect_Implementation(bool on)
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	PlayChargeEffect(on);
}

void UPFLasorCannonWeapon::PlayChargeEffect_Implementation(bool on)
{
	if (on)
	{
		// ���� ��ƼŬ ����Ʈ Ȱ��ȭ
		ChargeParticle1P->Activate(true);
		ChargeParticle3P->Activate(true);
	}
	else
	{
		// ���� ��ƼŬ ����Ʈ ��Ȱ��ȭ
		ChargeParticle1P->Deactivate();
		ChargeParticle3P->Deactivate();
	}
}

void UPFLasorCannonWeapon::PlayLasorEffect_Implementation(const FVector& start, const FVector& end, const float& lasorRadius)
{
	if (LasorNiagara != nullptr)
	{
		LasorNiagara->SetNiagaraVariableVec3(FString("StartLocation"), start);
		LasorNiagara->SetNiagaraVariableVec3(FString("EndLocation"), end);
		LasorNiagara->SetNiagaraVariableFloat(FString("InnerRadius"), lasorRadius * 0.8f);
		LasorNiagara->SetNiagaraVariableFloat(FString("OuterRadius"), lasorRadius * 2.0f);
		LasorNiagara->SetNiagaraVariableFloat(FString("SparkTangent"), lasorRadius * 0.0003);
		LasorNiagara->SetNiagaraVariableVec3(FString("SparkStartLocation"), start + ((end - start) * 0.02));
		LasorNiagara->ActivateSystem(true);
	}
	if (LasorSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(weaponOwner, LasorSound, weaponOwner->GetActorLocation());
	}
	
}