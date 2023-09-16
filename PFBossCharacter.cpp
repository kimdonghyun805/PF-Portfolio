// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBossCharacter.h"
#include "PFNormalCharacter.h"
#include "PFBossPlayerState.h"
#include "PFCoreObject.h"
#include "PF.h"
#include "PFGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponentPool.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraActor.h"

APFBossCharacter::APFBossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1��Ī �޽� ��ġ ����
	GetMesh1P()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh1P()->SetRelativeLocation(FVector(0.0f, 0.0f, -180.0f));
	GetMesh1P()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	// 3��Ī �޽� ��ġ ����
	GetMesh3P()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh3P()->SetRelativeLocation(FVector(0.0f, 0.0f, -180.0f));
	GetMesh3P()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	// 1��Ī ī�޶� ��ġ ����
	GetFirstPersonCameraComponent()->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));

	// ���� ĳ���� �Ӽ� ����
	GetCapsuleComponent()->SetCapsuleSize(110.f, 192.0f);
	GetCharacterMovement()->JumpZVelocity = defaultJumpVelocity;
	GetCharacterMovement()->MaxWalkSpeed = defaultWalkSpeed;

	GetCapsuleComponent()->SetCollisionProfileName("BlockAll");
	GetCapsuleComponent()->SetCollisionObjectType(BossCollisionChannel);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_Yes;

	// ���� ĳ���� ���̷�Ż �޽� ����
	//SkeletalMesh'/Game/PF/PFCharacter/ParagonCrouch/Mesh/Crunch_Black_Site.Crunch_Black_Site'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
		TEXT("SkeletalMesh'/Game/PF/PFCharacter/ParagonCrouch/Mesh/Crunch_Black_Site.Crunch_Black_Site'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		GetMesh3P()->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		GetMesh3P()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	// ���� ĳ���� �ִϸ��̼� ����
	//AnimBlueprint'/Game/PF/PFCharacter/ParagonCrouch/Animations/PFBossAnimBlueprint.PFBossAnimBlueprint'
	static ConstructorHelpers::FObjectFinder<UClass> animBlueprintObjectFinder(
		TEXT("AnimBlueprint'/Game/PF/PFCharacter/ParagonCrouch/Animations/PFBossAnimBlueprint.PFBossAnimBlueprint_C'"));
	if (animBlueprintObjectFinder.Succeeded())
	{
		//GetMesh3P()->SetAnimInstanceClass(animBlueprintObjectFinder.Object->GeneratedClass);
		GetMesh3P()->SetAnimClass(animBlueprintObjectFinder.Object);
	}
	// ���� �ִ� ��Ÿ�� ����
	//AnimMontage'/Game/PF/PFCharacter/ParagonCrouch/Animations/Ability_Combo_01_Montage.Ability_Combo_01_Montage'
	//AnimMontage'/Game/PF/PFCharacter/ParagonCrouch/Animations/Ability_Combo_02_Montage.Ability_Combo_02_Montage'
	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackAnimObj_0(
		TEXT("AnimMontage'/Game/PF/PFCharacter/ParagonCrouch/Animations/Ability_Combo_01_Montage.Ability_Combo_01_Montage'"));
	if (attackAnimObj_0.Succeeded())
	{
		AttackAnimMontage.Insert(attackAnimObj_0.Object, 0);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> attackAnimObj_1(
		TEXT("AnimMontage'/Game/PF/PFCharacter/ParagonCrouch/Animations/Ability_Combo_02_Montage.Ability_Combo_02_Montage'"));
	if (attackAnimObj_1.Succeeded())
	{
		AttackAnimMontage.Insert(attackAnimObj_1.Object, 1);
	}
	
	// ��ȭ ȿ�� ����Ʈ ����
	ReinforceNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ReinforceNiagara"));
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Reinforce.Reinforce'
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> reinforceNiagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Reinforce.Reinforce'"));
	if (reinforceNiagaraObj.Succeeded())
	{
		ReinforceNiagara->SetAsset(reinforceNiagaraObj.Object);
		ReinforceNiagara->SetupAttachment(GetCapsuleComponent());
		ReinforceNiagara->SetAutoDestroy(true);
		ReinforceNiagara->bAutoActivate = true;
		ReinforceNiagara->SetRelativeScale3D(FVector(5.0f, 5.0f, 1.0f));
		ReinforceNiagara->SetRelativeLocation(FVector(0.0f, 0.0f, -180.0f));
		ReinforceNiagara->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		ReinforceNiagara = nullptr;
	}
	// ��ȭ ȿ�� ��Ƽ���� ����
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind.MT_Wind'
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Green.MT_Wind_Green'
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Yellow.MT_Wind_Yellow'
	//Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Red.MT_Wind_Red'
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj_0(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind.MT_Wind'"));
	if (materialObj_0.Succeeded())
	{
		reinforceMaterial.Insert(materialObj_0.Object, 0);
	}
	else
	{
		reinforceMaterial.Insert(nullptr, 0);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj_1(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Green.MT_Wind_Green'"));
	if (materialObj_1.Succeeded())
	{
		reinforceMaterial.Insert(materialObj_1.Object, 1);
	}
	else
	{
		reinforceMaterial.Insert(nullptr, 1);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj_2(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Yellow.MT_Wind_Yellow'"));
	if (materialObj_2.Succeeded())
	{
		reinforceMaterial.Insert(materialObj_2.Object, 2);
	}
	else
	{
		reinforceMaterial.Insert(nullptr, 2);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> materialObj_3(
		TEXT("Material'/Game/PF/Effect/NaiagaraSystem/MT_Wind_Red.MT_Wind_Red'"));
	if (materialObj_3.Succeeded())
	{
		reinforceMaterial.Insert(materialObj_3.Object, 3);
	}
	else
	{
		reinforceMaterial.Insert(nullptr, 3);
	}
	// �Ϲ� ���� ����Ʈ ����
	AttackNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AttackNiagara"));
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/BossAttack.BossAttack'
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> attackNiagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/BossAttack.BossAttack'"));
	if (attackNiagaraObj.Succeeded())
	{
		AttackNiagara->SetAsset(attackNiagaraObj.Object);
		AttackNiagara->SetAutoDestroy(true);
		AttackNiagara->bAutoActivate = false;
	}
	else
	{
		AttackNiagara = nullptr;
	}
	// ����� ����Ʈ ����
	ShockwaveNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShockwaveNiagara"));
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Shockwave.Shockwave'
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> shockwaveNiagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Shockwave.Shockwave'"));
	if (shockwaveNiagaraObj.Succeeded())
	{
		ShockwaveNiagara->SetAsset(shockwaveNiagaraObj.Object);
		ShockwaveNiagara->SetAutoDestroy(true);
		ShockwaveNiagara->bAutoActivate = false;
	}
	else
	{
		ShockwaveNiagara = nullptr;
	}
	// ���� ����Ʈ ����
	ChargeNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChargeNiagara"));
	//NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Charge.Charge'
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> crashNiagaraObj(
		TEXT("NiagaraSystem'/Game/PF/Effect/NaiagaraSystem/Charge.Charge'"));
	if (crashNiagaraObj.Succeeded())
	{
		ChargeNiagara->SetAsset(crashNiagaraObj.Object);
		ChargeNiagara->SetAutoDestroy(true);
		ChargeNiagara->bAutoActivate = false;
	}
	else
	{
		ChargeNiagara = nullptr;
	}

	// �Ϲ� ���� ���� ����
	//SoundCue'/Game/PF/Audio/Boss/BossAttack.BossAttack'
	static ConstructorHelpers::FObjectFinder<USoundCue> attackSoundObj(
		TEXT("SoundCue'/Game/PF/Audio/Boss/BossAttack.BossAttack'"));
	if (attackSoundObj.Succeeded())
	{
		AttackSound = attackSoundObj.Object;
	}
	else
	{
		AttackSound = nullptr;
	}
	// ���� ���� ����
	//SoundCue'/Game/PF/Audio/Boss/BossCrash.BossCrash'
	static ConstructorHelpers::FObjectFinder<USoundCue> chargeSoundObj(
		TEXT("SoundCue'/Game/PF/Audio/Boss/BossCrash.BossCrash'"));
	if (chargeSoundObj.Succeeded())
	{
		ChargeSound = chargeSoundObj.Object;
	}
	else
	{
		ChargeSound = nullptr;
	}
	// ����� ���� ����
	//SoundCue'/Game/PF/Audio/Boss/BossShockwave.BossShockwave'
	static ConstructorHelpers::FObjectFinder<USoundCue> shockwaveSoundObj(
		TEXT("SoundCue'/Game/PF/Audio/Boss/BossShockwave.BossShockwave'"));
	if (shockwaveSoundObj.Succeeded())
	{
		ShockwaveSound = shockwaveSoundObj.Object;
	}
	else
	{
		ShockwaveSound = nullptr;
	}

	// ���� ĳ���� ���� �ʱ�ȭ
	Reinforce(0);
	numAttack = 0;
}

void APFBossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(InputComponent);
	Super::SetupPlayerInputComponent(InputComponent);
	// ���� ��ų�� ���Ͽ� jump �Է��� ���Ӱ� ���ε�
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APFBossCharacter::StartJumpKey);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APFBossCharacter::EndJumpKey);
	// ���� ��ų�� ���Ͽ� Skill �Է��� ���ε�
	PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &APFBossCharacter::StartChargeKey);
	PlayerInputComponent->BindAction("Skill", IE_Released, this, &APFBossCharacter::EndChargeKey);
	// ���� �⺻������ ���Ͽ� PrimaryAction�� �������̵��Ͽ� ���ε�
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APFBossCharacter::OnPrimaryAction);
	// ��Ÿ ��ȣ�ۿ��� ���Ͽ� Interaction�� ���ε�
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &APFBossCharacter::SetOnInteraction);
	PlayerInputComponent->BindAction("Interaction", IE_Released, this, &APFBossCharacter::SetOffInteraction);
}

void APFBossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (bIsJumpCharging && (jumpChargeValue < jumpMaxChargeValue))
		{
			jumpChargeValue += DeltaTime * jumpChargePerSec;
		}
		if (bIsCharging)
		{
			ServerLaunchCharacter(chargeDirection * DeltaTime);
			chargeTime += DeltaTime;
			if (chargeTime > chargeMaxTime) // ���� �ð� ����� ���� ��ų ����
			{
				EndChargeKey();
			}
		}
		if (attackDelay > 0.0f)
		{
			attackDelay -= DeltaTime;
		}
		if (chargeCoolDown > 0.0f)
		{
			chargeCoolDown -= DeltaTime;
		}
		if (jumpCoolDown > 0.0f)
		{
			jumpCoolDown -= DeltaTime;
		}
	}
}

void APFBossCharacter::ServerLaunchCharacter_Implementation(const FVector& dir) 
{
	LaunchCharacter(dir, false, false);
}

void APFBossCharacter::BeginPlay()
{
	Super::BeginPlay();

}

float APFBossCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventIntigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventIntigator, DamageCauser);
	if (GetLocalRole() == ROLE_Authority && BossPlayerState != nullptr)
	{
		// ĳ������ ���� ��ŭ �Դ� ���� ����
		float damage = Damage - armor;
		if (damage <= 0.0f)
		{
			damage = 0;
		}
		
		DecreaseHealth(damage);
		if (GetHealth() <= 0.0f)
		{
			APFGameMode* gameMode = GetWorld()->GetAuthGameMode<APFGameMode>();
			if (gameMode->IsGameStart())
			{
				BossKilled(false);
			}
			else
			{
				// ���� ���� ���� �ƴ� ���, ü���� ��� ȸ���ϰ� ������
				IncreaseHealth(GetMaxHealth());
				BossKilled(true); 
			}
		}
		return damage;
	}
	return Damage;
}

void APFBossCharacter::OnPrimaryAction()
{
	// �⺻ ���� Ű�� �Է��Ͽ� �⺻���� ����
	Super::OnPrimaryAction();
	if (!bIsCharging && attackDelay <= 0.0f)
	{
		FVector mouseDir = GetMouseDirection();
		mouseDir.Normalize();
		FRotator rot = GetController()->GetControlRotation();
		ServerBasicAttack(mouseDir, rot, attackDamage, attackRange, attackRadius);
		attackDelay = attackMaxDelay;
	}
	
}

void APFBossCharacter::Landed(const FHitResult& Hit)
{
	// ���� �� ���Ͻ� ����
	// ���� �� �̻� ������ ������ ���, �ֺ� �浹 ��ġ�� ���� ����
	Super::Landed(Hit);
	if (!bIsJumpCharging && jumpChargeValue > 0.0f)
	{
		ServerCrash(Hit, crashDamage);
		FRotator rot = GetController()->GetControlRotation();
		ServerShockWave(true, rot, shockDamage, shockRange, jumpChargeValue, bIsCharging);
		// ���� �������� ���� ������ �� ����
		jumpChargeValue = 0.0f;
		ServerSetJumpVariable(false, jumpChargeValue, defaultJumpVelocity);
		// ���� ������ �缳��
		attackDelay = attackMaxDelay;
	}
}

void APFBossCharacter::MoveBlockedBy(const FHitResult& Impact)
{
	// ĳ������ �������� �浹�� ���� ���� �� ����
	// ���� ��ų ��� ���� �� �浹 ��, �Ϲ� ĳ���͸� ���ĳ��� ���� ���ظ� �ְ�, ���� ��ų ����
	Super::MoveBlockedBy(Impact);
	if (bIsCharging)
	{
		ServerCrash(Impact, crashDamage);
		EndChargeKey();
	}
}

void APFBossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController->GetPlayerState<APlayerState>() != nullptr)
	{
		if (NewController->GetPlayerState<APlayerState>()->GetClass()->IsChildOf<APFBossPlayerState>())
		{
			SetBossPlayerState(NewController->GetPlayerState<APFBossPlayerState>());
			if (BossPlayerState != nullptr)
			{
				Reinforce(BossPlayerState->GetReinforceLevel());
			}
		}
	}
}

void APFBossCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	// ���� ��(KillZ �̸�)���� ������ �� ����
	// Super �Լ�(ĳ���͸� �Ҹ���)�� �������� ����
	//Super::FellOutOfWorld(dmgType);
	if (!(BossPlayerState->IsObserving()))
	{
		BossKilled(true);
	}
}

void APFBossCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void APFBossCharacter::StartJumpKey()
{
	// ���� ��ų Ű �Է� �� ����
	// ���� ��, ���� ��ų ���� ���� �ƴ� ���, ���� ��ų ����
	// ��ų ��Ÿ�� ���� ���, �Ϲ� ���� ����
	if (!(GetCharacterMovement()->IsFalling()) && !bIsCharging)
	{
		if (jumpCoolDown <= 0.0f)
		{
			bIsJumpCharging = true;
		}
		else
		{
			Jump();
		}
	}
}

void APFBossCharacter::EndJumpKey()
{
	// ���� ��ų Ű �Է� ���� �� ����
	// ���� ��ų ���� ���� ���, ������ �����ϰ� ��ȭ�� ���� ����
	if (bIsJumpCharging) // ���� ���� ���� ���� ����
	{
		// �������� ���� ���� ���� ����
		ServerSetJumpVariable(true, jumpChargeValue, defaultJumpVelocity);
		Jump();
		// ���� ���� ����
		bIsJumpCharging = false;
		// ���� ��ų ��Ÿ�� ����
		jumpCoolDown = jumpMaxCoolDown;
	}
	else
	{
		StopJumping();
	}
}

void APFBossCharacter::ServerSetJumpVariable_Implementation(bool on, const float& jumpCharge, const float& jumpVelocityDefault)
{
	SetJumpVariable(on, jumpCharge, jumpVelocityDefault);
}

void APFBossCharacter::SetJumpVariable_Implementation(bool on, const float& jumpCharge, const float& jumpVelocityDefault)
{
	if (on)
	{
		GetCharacterMovement()->JumpZVelocity += jumpCharge;
	}
	else
	{
		GetCharacterMovement()->JumpZVelocity = jumpVelocityDefault;
	}
}


void APFBossCharacter::StartChargeKey()
{
	// ���� ��ų Ű �Է� �� ����
	// �������� ����
	if (!bIsCharging && chargeCoolDown <= 0.0f)
	{
		bIsCharging = true;
		// ���� ���� ����
		ServerSetChargeVariable(true, chargeDirection, chargeWalkSpeed, defaultWalkSpeed);
		// �÷��̾ �ٶ󺸴� ���� ���
		FVector mouseDir = GetMouseDirection();
		if (!(GetCharacterMovement()->IsFalling()))
		{
			// ���� ���� �ƴ� ���, ������ ���� �����ϵ��� z���� ����
			mouseDir.Z = 0.0f;
		}
		chargeDirection = mouseDir * chargeSpeed;
		chargeCoolDown = chargeMaxCoolDown;
	}
}

void APFBossCharacter::ServerSetChargeVariable_Implementation(bool on, const FVector& direction, const float& walkSpeedCharge, const float& walkSpeedDefault)
{
	SetChargeVariable(on, direction, walkSpeedCharge, walkSpeedDefault);
}

void APFBossCharacter::SetChargeVariable_Implementation(bool on, const FVector& direction, const float& walkSpeedCharge, const float& walkSpeedDefault)
{
	if (on)
	{
		GetCharacterMovement()->MaxWalkSpeed += walkSpeedCharge;
		// ���� ������ ��߳��� �ʵ��� �ܷ��� ����
		GetCharacterMovement()->GravityScale = 0.0f;
	}
	else
	{
		// ���� ��ų�� ���� ������ �� ����
		GetCharacterMovement()->MaxWalkSpeed = walkSpeedDefault;
		GetCharacterMovement()->GravityScale = 1.0f;
		// ���� ��ų ������� ���� ������ �ӵ��� ��ȭ -> �ڿ������� ������
		GetCharacterMovement()->Velocity = direction;
		GetCharacterMovement()->PendingLaunchVelocity = direction;
	}
}

void APFBossCharacter::EndChargeKey()
{
	// ���� ��ų Ű �Է� ���� �� ����
	// ���� ��ų�� ��� �����ϰ� ����Ŀ� ���� ����
	if (bIsCharging) // ���� ��ų ���� ���� ��
	{
		// �ֺ� ���� ������ �����
		FRotator rot = GetController()->GetControlRotation();
		ServerShockWave(false, rot, shockDamage, shockRange, jumpChargeValue, bIsCharging);
		// ���� ��ų�� ���� ������ ���� ����
		bIsCharging = false;
		chargeTime = 0.0f;
		ServerSetChargeVariable(false, chargeDirection, chargeWalkSpeed, defaultWalkSpeed);
		// ���� ������ �缳��
		attackDelay = attackMaxDelay;
	}
}

void APFBossCharacter::Crash(const FHitResult& Hit, const float& damage)
{
	// ����, ���� ��ų ���� ��, ���� ���������� �浹 �� ����
	// ���� �浹�� �Ϲ� �÷��̾� �� �ھ�� �߰� ����
	if (Hit.GetActor() != nullptr)
	{
		if (Hit.GetActor()->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(Hit.GetActor());
			if (nc != nullptr)
			{
				FDamageEvent crashEvent(UDamageType::StaticClass());
				nc->TakeDamage(damage, crashEvent, GetController(), this);
			}
		}
		else if (Hit.GetActor()->GetClass()->IsChildOf<APFCoreObject>())
		{
			APFCoreObject* core = Cast<APFCoreObject>(Hit.GetActor());
			if (core != nullptr)
			{
				FDamageEvent crashEvent(UDamageType::StaticClass());
				core->TakeDamage(damage, crashEvent, GetController(), this);
			}
		}
	}
}

void APFBossCharacter::ShockWave(bool activateEffect, const FRotator& rotation, 
	const float& damage, const float& range, const float& jumpCharge, bool isCharging)
{
	// ���� ���� ����� �߻�, ���� ���ظ� �ְ� ���ĳ�
	// ��ü ���� ���� �Ϲ� ĳ����, �ھ� ������Ʈ�� Ž��
	TArray<AActor*> target = SweepSphere(range, GetActorLocation(), GetActorLocation() + FVector(0, 0, 1.0f));
	// �浹�� ���Ϳ��� ����
	FDamageEvent shockEvent(UDamageType::StaticClass());
	for (auto& t : target)
	{
		t->TakeDamage(damage, shockEvent, GetController(), this);
		// �Ϲ� ĳ������ ��� �˹�
		if (t->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(t);
			// �˹� ���� ���
			FVector knockbackDir;
			knockbackDir = nc->GetActorLocation() + (-GetActorLocation());

			int weight = ((int)range - (int)knockbackDir.Length()) / 2 + 100;
			knockbackDir.Normalize(); // ����ȭ
			if (!(nc->GetCharacterMovement()->IsFalling()))
			{
				knockbackDir.Z += 1.0f; // ���� ���� �ƴ� ���, �Ϲ� ĳ���͸� �پ� �ø�
			}
			if (GetCharacterMovement()->IsFalling() && jumpCharge != 0.0f)
			{
				// ���� ��ų�� �˹� ����, jumpChargeValue : 100 ~ 800
				if (jumpCharge > 400.0f)
				{
					weight *= (int)jumpCharge / 400;
				}
			}
			if (isCharging)
			{
				// ���� ��ų�� �˹� ����
				weight *= 2;
			}
			nc->GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
			nc->GetCharacterMovement()->PendingLaunchVelocity = FVector(0.0f, 0.0f, 0.0f);
			nc->LaunchCharacter(knockbackDir* weight, false, false);
		}
	}
	// activateEffect�� true�� ���, ���� ����� ����Ʈ ����
	// false�� ���, ���� ����� ����Ʈ ����
	if (activateEffect)
	{
		FVector loc = GetActorLocation() - FVector(0.0f, 0.0f, 150.0f);
		PlayShockwaveEffect(loc);
	}
	else
	{
		FVector loc = GetActorLocation() + (GetActorForwardVector() * 400.0f);
		FRotator rot = rotation;
		PlayChargeEffect(loc, rot);
	}
	
}

void APFBossCharacter::BasicAttack(const FVector& mouseDir, const FRotator& rotation, 
	const float& damage, const float& range, const float& radius)
{
	// �⺻ ���� Ű �Է� �� ����
	// ĳ���Ͱ� �ٶ󺸴� �������� ��ǳ
	FVector start = GetActorLocation() + (mouseDir * 150);
	FVector end = GetActorLocation() + (mouseDir * range);
	TArray<AActor*> target = SweepSphere(radius, start, end);
	FDamageEvent attackEvent(UDamageType::StaticClass());
	for (auto& t : target)
	{
		t->TakeDamage(damage, attackEvent, GetController(), this);
		// �Ϲ� ĳ������ ��� �ణ�� �˹�
		if (t->GetClass()->IsChildOf<APFNormalCharacter>())
		{
			APFNormalCharacter* nc = Cast<APFNormalCharacter>(t);
			FVector knockbackDir = mouseDir;
			int weight;
			if (nc->GetCharacterMovement()->IsFalling()) { weight = 2000; }
			else { weight = 2000; }
			knockbackDir.Z = 0.0f;
			nc->LaunchCharacter(knockbackDir * weight, false, false);
		}
	}
	// �Ϲ� ���� �ִϸ��̼� ����
	PlayAttackAnim();
	// �Ϲ� ���� ����Ʈ ����
	PlayAttackEffect(rotation);
}

FVector APFBossCharacter::GetMouseDirection()
{
	// �÷��̾��� ���콺�� �ٶ󺸴� ���� ���
	FVector mousePos, mouseDir;
	APlayerController* controller = Cast<APlayerController>(GetController());
	FVector2D screenPos = GEngine->GameViewport->Viewport->GetSizeXY();
	controller->DeprojectScreenPositionToWorld(screenPos.X / 2.0f, screenPos.Y / 2.0f, mousePos, mouseDir);
	return mouseDir;
}

TArray<AActor*> APFBossCharacter::SweepSphere(const float& radius, const FVector& start, const FVector& end)
{
	// ��ü�� sweep ����Ͽ� ���� ������ �Ϲ� ĳ���Ϳ� �ھ Ž��
	TArray<AActor*> targetArray;
	FCollisionObjectQueryParams targetObj;
	targetObj.AddObjectTypesToQuery(NormalCollisionChannel);
	targetObj.AddObjectTypesToQuery(CoreCollisionChannel);
	FCollisionQueryParams thisObj(FName(TEXT("SweepSphere")), true, this);
	TArray<FHitResult> hitResult;
	GetWorld()->SweepMultiByObjectType(hitResult, start, end, FQuat(),
		targetObj, FCollisionShape::MakeSphere(radius), thisObj);
	for (auto& hit : hitResult)
	{
		if (hit.GetActor()->GetClass()->IsChildOf<APFNormalCharacter>()) // ������ ������Ʈ�� Ÿ�� ������� Ȯ��
		{
			targetArray.Emplace(hit.GetActor());
		}
		else if (hit.GetActor()->GetClass()->IsChildOf<APFCoreObject>())
		{
			targetArray.Emplace(hit.GetActor());
		}
	}

	// ����� ��ü �׸���
	/*FColor col = FColor::Yellow;
	//if (numAttackedObject) { col = FColor::Green; }
	//else { col = FColor::Red; }
	DrawDebugSphere(GetWorld(), start, radius, 12, col, false, 10.0f);
	DrawDebugSphere(GetWorld(), end, radius, 12, col, false, 10.0f);*/
	return targetArray;
}

void APFBossCharacter::ServerReinforce_Implementation(const uint8& level)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	Reinforce(level);
	PlayReinforceEffect();
	
}

void APFBossCharacter::Reinforce_Implementation(const uint8& level)
{
	// ���� ĳ������ �ɷ�ġ ��ȭ
	if (level >= 0 && level <= 3)
	{
		defaultJumpVelocity = arr_defaultJumpVelocity[level];
		defaultWalkSpeed = arr_defaultWalkSpeed[level];
		armor = arr_armor[level];

		chargeMaxTime = arr_chargeMaxTime[level];
		chargeSpeed = arr_chargeSpeed[level];
		chargeMaxCoolDown = arr_chargeMaxCoolDown[level];
		chargeEffectScale = FVector(arr_chargeEffectMag[level] * 0.5f, arr_chargeEffectMag[level], arr_chargeEffectMag[level]);

		jumpChargePerSec = arr_jumpChargePerSec[level];
		jumpMaxCoolDown = arr_jumpMaxCoolDown[level];

		crashDamage = arr_crashDamage[level];
		shockDamage = arr_shockDamage[level];
		shockRange = arr_shockRange[level];
		shockEffectScale = FVector(arr_shockEffectMag[level], arr_shockEffectMag[level], 1.0f);

		attackDamage = arr_attackDamage[level];
		attackRange = arr_attackRange[level];
		attackMaxDelay = arr_attackMaxDelay[level];
		attackEffectScale = FVector(arr_attackEffectMag[level], 8.0f, 8.0f);

		GetCharacterMovement()->JumpZVelocity = defaultJumpVelocity;
		GetCharacterMovement()->MaxWalkSpeed = defaultWalkSpeed;

		if (ReinforceNiagara != nullptr && reinforceMaterial[level] != nullptr)
		{
			ReinforceNiagara->SetVariableMaterial(FName("ColorMaterial"), reinforceMaterial[level]);
		}
	}
	
}

void APFBossCharacter::ReinforceNext_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (BossPlayerState != nullptr)
	{
		uint8 reinforceLevel = BossPlayerState->GetReinforceLevel();
		if (reinforceLevel >= -1 && reinforceLevel <= 2)
		{
			BossPlayerState->IncreaseReinforceLevel();
		}
	}
	
}

void APFBossCharacter::BossKilled(bool canRespawn)
{
	// ���� ĳ���� ��� �� ����
	PlayRagdoll();
	DetachFromControllerPendingDestroy();
	if (canRespawn)
	{
		// ������ ����
		FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
		worldTimerManager.SetTimer(respawnTimerHandle, this, &APFBossCharacter::BossRespawn, respawnTime, false);
	}
	else
	{
		// ������ �Ұ���, �÷��� ����
		GetWorld()->GetAuthGameMode<APFGameMode>()->BossFinished(BossPlayerState);
		// ���׵� ����
		FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
		worldTimerManager.SetTimer(respawnTimerHandle, this, &APFBossCharacter::BossRagdollDestroy, respawnTime, false);
	}
	// ���� ���� ����
	BossPlayerState->Observing();
}

void APFBossCharacter::BossRespawn()
{
	// ���� ĳ���͸� ������ �ϰ� �ڽ��� �Ҹ�
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(respawnTimerHandle);
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	APlayerController* bossController = Cast<APlayerController>(BossPlayerState->GetBossController());
	gameMode->BossRespawn(bossController);
	Destroy();
}

void APFBossCharacter::BossRagdollDestroy()
{
	// ���� ĳ���͸� ���������� �ʰ�, �ڽ��� �Ҹ�
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(respawnTimerHandle);
	Destroy();
}

APFBossPlayerState* APFBossCharacter::GetBossPlayerState()
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState;
	}
	else
	{
		BossPlayerState = Cast<APFBossPlayerState>(GetPlayerState());
		return BossPlayerState;
	}
}

void APFBossCharacter::SetBossPlayerState(APFBossPlayerState* newPS)
{
	if (newPS != nullptr)
	{
		BossPlayerState = newPS;
	}
}

void APFBossCharacter::ServerCrash_Implementation(const FHitResult& Hit, const float& damage)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	Crash(Hit, damage);
}

bool APFBossCharacter::ServerCrash_Validate(const FHitResult& Hit, const float& damage)
{
	if (!(Hit.bBlockingHit))
	{
		return false;
	}
	if (damage < 0.0f)
	{
		return false;
	}
	return true;
}

void APFBossCharacter::ServerShockWave_Implementation(bool activateEffect, const FRotator& rotation, 
	const float& damage, const float& range, const float& jumpCharge, bool isCharging)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	ShockWave(activateEffect, rotation, damage, range, jumpCharge, isCharging);
}

bool APFBossCharacter::ServerShockWave_Validate(bool activateEffect, const FRotator& rotation, 
	const float& damage, const float& range, const float& jumpCharge, bool isCharging)
{
	if ((damage < 0.0f || range < 0.0f) || jumpCharge < 0.0f)
	{
		return false;
	}
	return true;
}

void APFBossCharacter::ServerBasicAttack_Implementation(const FVector& mouseDir, const FRotator& rotation, 
	const float& damage, const float& range, const float& radius)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	BasicAttack(mouseDir, rotation, damage, range, radius);
	attackDelay = attackMaxDelay;
}

bool APFBossCharacter::ServerBasicAttack_Validate(const FVector& mouseDir, const FRotator& rotation, 
	const float& damage, const float& range, const float& radius)
{
	if ((damage < 0.0f || range < 0.0f) || radius < 0.0f)
	{
		return false;
	}
	return true;
}

void APFBossCharacter::PlayAttackAnim_Implementation()
{
	// �Ϲ� ���� �ִϸ��̼� ����
	if (AttackAnimMontage[0] != nullptr && AttackAnimMontage[1] != nullptr)
	{
		if (numAttack % 2 == 0)
		{
			PlayAnimMontage(AttackAnimMontage[0]);
		}
		else
		{
			PlayAnimMontage(AttackAnimMontage[1]);
		}
		++numAttack;
	}
}

void APFBossCharacter::PlayReinforceEffect_Implementation()
{
	// ��ȭ ȿ�� ����Ʈ ����
	if (ReinforceNiagara != nullptr)
	{
		ReinforceNiagara->Activate(true);
	}
}

void APFBossCharacter::PlayAttackEffect_Implementation(const FRotator& rotation)
{
	// �Ϲ� ���� ����Ʈ ���
	if (AttackNiagara != nullptr)
	{
		FRotator rot = rotation;
		FVector loc = GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackNiagara->GetAsset(), loc, rot, attackEffectScale);
	}
	if (AttackSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	}
}

void APFBossCharacter::PlayShockwaveEffect_Implementation(const FVector& location)
{
	// ����� ����Ʈ ���
	if (ShockwaveNiagara != nullptr)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShockwaveNiagara->GetAsset(), location, FRotator(0.0f, 0.0f, 0.0f), shockEffectScale);
	}
	if (ShockwaveSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShockwaveSound, GetActorLocation());
	}
}

void APFBossCharacter::PlayChargeEffect_Implementation(const FVector& location, const FRotator& rotation)
{
	// ���� ����Ʈ ���
	if (ChargeNiagara != nullptr)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ChargeNiagara->GetAsset(), location, rotation, chargeEffectScale);
	}
	if (ChargeSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ChargeSound, GetActorLocation());
	}
}

void APFBossCharacter::SetOnInteraction_Implementation()
{
	if (BossPlayerState != nullptr)
	{
		BossPlayerState->SetOnInteraction();
	}
}
void APFBossCharacter::SetOffInteraction_Implementation()
{
	if (BossPlayerState != nullptr)
	{
		BossPlayerState->SetOffInteraction();
	}
}
bool APFBossCharacter::IsOnInteraction()
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->IsOnInteraction();
	}
	return false;
}
void APFBossCharacter::SetHealth_Implementation(const float& value)
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->SetHealth(value);
	}
}

void APFBossCharacter::DecreaseHealth_Implementation(const float& value)
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->DecreaseHealth(value);
	}
}

void APFBossCharacter::IncreaseHealth_Implementation(const float& value)
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->IncreaseHealth(value);
	}
}

float APFBossCharacter::GetHealth() const
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->GetHealth();
	}
	return -1.0f;
}
float APFBossCharacter::GetMaxHealth() const
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->GetMaxHealth();
	}
	return -1.0f;
}
uint8 APFBossCharacter::GetReinforceLevel() const
{
	if (BossPlayerState != nullptr)
	{
		return BossPlayerState->GetReinforceLevel();
	}
	return -1;
}
