// Fill out your copyright notice in the Description page of Project Settings.


#include "PFWeapon.h"
#include "PF.h"
#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"

UPFWeapon::UPFWeapon()
{
	PrimaryComponentTick.bCanEverTick = true;

	Mesh1P = nullptr;
	Mesh3P = nullptr;

	dynamicMaterial3P = nullptr;
	dynamicMaterial1P = nullptr;

	swapDelay = 0.0f;
	maxSwapDelay = 1.0f;
	swapMovement = FVector(0.0f, 0.0f, 30.0f);

	CharacterReloadAnimMontage = nullptr;
	CharacterFireAnimMontage = nullptr;
	CharacterEquipAnimMontage = nullptr;
	WeaponReloadAnimMontage = nullptr;
	WeaponFireAnimMontage = nullptr;

	FireParticle1P = nullptr;
	FireParticle3P = nullptr;

}

void UPFWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPFWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (weaponOwner->GetLocalRole() == ROLE_AutonomousProxy && bIsPicked)
	{
		if (fireDelay > 0.0f)
		{
			fireDelay -= DeltaTime;
		}

		if (bIsReloading)
		{
			if (remainReloadingTime < 0.0f)
			{
				LoadAmmo();
			}
			else
			{
				remainReloadingTime -= DeltaTime;
			}

		}
		if (swapDelay > 0.0f)
		{
			swapDelay -= DeltaTime;
		}
	}

	if (weaponOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		// 무장 교체 연출 수행
		if (swapDelay > 0.0f && GetMesh1P() != nullptr)
		{
			GetMesh1P()->AddRelativeLocation(swapMovement * DeltaTime / maxSwapDelay);
		}
	}

}

void UPFWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPFWeapon::OnFire(const FVector& mouseDir, const FVector& mousePos)
{
	//
}

void UPFWeapon::AfterFire(const FVector& mouseDir, const FVector& mousePos)
{
	//
}

void UPFWeapon::Reload()
{
	//
}

void UPFWeapon::LoadAmmo()
{
	//
}

AActor* UPFWeapon::LineTrace(const FVector& start, const FVector& end)
{
	AActor* target = nullptr;
	FCollisionObjectQueryParams targetObj;
	FCollisionQueryParams thisObj(FName(TEXT("WeaponFire")), true, weaponOwner);
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, targetObj, thisObj);
	bool bIsHit = false;
	if (hitResult.bBlockingHit) // 충돌이 발생한 경우
	{
		if (hitResult.GetActor()->GetClass()->IsChildOf<APFBossCharacter>())
		{
			target = hitResult.GetActor();
			bIsHit = true;
		}
	}
	// 디버그 라인 그리기
	FColor col = FColor::Yellow;
	if (bIsHit) { col = FColor::Green; }
	else { col = FColor::Red; }
	DrawDebugLine(GetWorld(), start, end, col, false, 10.0f, 0, 0.0f);
	return target;
}

void UPFWeapon::ServerPickThisWeapon_Implementation()
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	PickThisWeapon();
	PlayEquipAnim();
}

void UPFWeapon::PickThisWeapon()
{
	bIsPicked = true;
	swapDelay = maxSwapDelay; // 무장 교체 딜레이 설정
	GetMesh1P()->SetVisibility(true); // 메시 보이기
	GetMesh3P()->SetVisibility(true);
}

void UPFWeapon::ServerUnpickThisWeapon_Implementation()
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	UnpickThisWeapon();
}

void UPFWeapon::UnpickThisWeapon()
{
	bIsPicked = false;
	bIsReloading = false;
	remainReloadingTime = 0.0f;
	GetMesh1P()->SetVisibility(false); // 메시 숨기기
	GetMesh3P()->SetVisibility(false);
	GetMesh1P()->SetRelativeLocation(meshLocation1P);
}

void UPFWeapon::ServerPlayReloadAnim_Implementation()
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	PlayReloadAnim();
}

void UPFWeapon::ServerPlayFireAnim_Implementation()
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	PlayFireAnim();
}

void UPFWeapon::ServerPlayEquipAnim_Implementation()
{
	if (weaponOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	PlayEquipAnim();
}

void UPFWeapon::PlayReloadAnim_Implementation()
{
	if (CharacterReloadAnimMontage != nullptr && WeaponReloadAnimMontage != nullptr)
	{
		weaponOwner->PlayAnimMontage(CharacterReloadAnimMontage);
		Mesh3P->PlayAnimation(WeaponReloadAnimMontage, false);
		Mesh1P->PlayAnimation(WeaponReloadAnimMontage, false);
	}
}

void UPFWeapon::PlayFireAnim_Implementation()
{
	if (CharacterFireAnimMontage != nullptr)
	{
		weaponOwner->PlayAnimMontage(CharacterFireAnimMontage);
		Mesh3P->PlayAnimation(WeaponFireAnimMontage, false);
		Mesh1P->PlayAnimation(WeaponFireAnimMontage, false);
	}
	if (FireParticle1P != nullptr && FireParticle3P != nullptr)
	{
		FireParticle1P->Activate(true);
		FireParticle3P->Activate(true);
	}
}

void UPFWeapon::PlayEquipAnim_Implementation()
{
	if (CharacterEquipAnimMontage != nullptr)
	{
		weaponOwner->PlayAnimMontage(CharacterEquipAnimMontage);
	}
}

void UPFWeapon::SetWeaponColor_Implementation(const FLinearColor& color)
{
	if (dynamicMaterial3P != nullptr && dynamicMaterial1P != nullptr)
	{
		dynamicMaterial3P->SetVectorParameterValue("WeaponColor", color);
		dynamicMaterial1P->SetVectorParameterValue("WeaponColor", color);
	}
}

void UPFWeapon::ServerSetWeaponOwner_Implementation(APFNormalCharacter* owner)
{
	SetWeaponOwner(owner);
}

void UPFWeapon::SetWeaponOwner_Implementation(APFNormalCharacter* owner)
{
	weaponOwner = owner;
	GetMesh1P()->AttachToComponent(weaponOwner->GetFirstPersonCameraComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	GetMesh3P()->AttachToComponent(weaponOwner->GetMesh3P(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_rSocket"));
}

void UPFWeapon::SetRemainAmmo(const uint8& value)
{
	if (value > 0)
	{
		remainAmmo = value;
	}
	else
	{
		remainAmmo = 0;
	}
}
void UPFWeapon::IncreaseRemainAmmo(const uint8& value)
{
	if (value > 0)
	{
		if (remainAmmo + value > maxRemainAmmo)
		{
			remainAmmo = maxRemainAmmo;
		}
		else
		{
			remainAmmo += value;
		}
	}
}
void UPFWeapon::SetLoadedAmmo(const uint8& value)
{
	if (value > 0)
	{
		loadedAmmo = value;
	}
	else
	{
		loadedAmmo = 0;
	}
	
}
void UPFWeapon::IncreaseLoadedAmmo(const uint8& value)
{
	if (value > 0)
	{
		if (loadedAmmo + value > maxLoadedAmmo)
		{
			loadedAmmo = maxLoadedAmmo;
		}
		else
		{
			loadedAmmo += value;
		}
	}
}
void UPFWeapon::SetSwapDelay(const float& value)
{
	swapDelay = value;
}
bool UPFWeapon::IsAmmoEmpty()
{
	if (loadedAmmo == 0 && remainAmmo == 0)
	{
		return true;
	}
	return false;
}