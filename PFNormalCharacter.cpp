// Fill out your copyright notice in the Description page of Project Settings.


#include "PFNormalCharacter.h"
#include "PFBossCharacter.h"
#include "PFNormalPlayerState.h"
#include "PF.h"
#include "PFWeapon.h"
#include "PFPistolWeapon.h"
#include "PFRifleWeapon.h"
#include "PFLasorCannonWeapon.h"
#include "PFGrenadeWeapon.h"
#include "PFGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/EngineTypes.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "TimerManager.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

APFNormalCharacter::APFNormalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1인칭 메시 위치 조정
	GetMesh1P()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh1P()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh1P()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	// 3인칭 메시 위치 조정
	GetMesh3P()->SetRelativeRotation(FRotator(0.0f, -95.0f, 0.0f));
	GetMesh3P()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh3P()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	GetMesh3P()->SetIsReplicated(true);
	// 1인칭 카메라 위치 조정
	GetFirstPersonCameraComponent()->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	
	// 일반 캐릭터 속성 설정
	GetCharacterMovement()->JumpZVelocity = defaultJumpVelocity;
	GetCharacterMovement()->MaxWalkSpeed = defaultWalkSpeed;

	GetCapsuleComponent()->SetCapsuleSize(55.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName("BlockAll");
	GetCapsuleComponent()->SetCollisionObjectType(NormalCollisionChannel);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;

	// 일반 캐릭터 스켈레탈 메시 설정
	//SkeletalMesh'/Game/PF/PFCharacter/Mannequins/Mesh/SKM_Manny.SKM_Manny'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshObjectFinder(
		TEXT("SkeletalMesh'/Game/PF/PFCharacter/Mannequins/Mesh/SKM_Manny.SKM_Manny'"));
	if (skeletalMeshObjectFinder.Succeeded())
	{
		GetMesh3P()->SetSkeletalMesh(skeletalMeshObjectFinder.Object);
		GetMesh3P()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	// 일반 캐릭터 애니메이션 설정
	//AnimBlueprint'/Game/PF/PFCharacter/Mannequins/Animations/PFNormalAnimBlueprint.PFNormalAnimBlueprint'
	static ConstructorHelpers::FObjectFinder<UClass> animBlueprintObjectFinder(
		TEXT("AnimBlueprint'/Game/PF/PFCharacter/Mannequins/Animations/PFNormalAnimBlueprint.PFNormalAnimBlueprint_C'"));
	if (animBlueprintObjectFinder.Succeeded())
	{
		//GetMesh3P()->SetAnimInstanceClass(animBlueprintObjectFinder.Object->GeneratedClass);
		GetMesh3P()->SetAnimClass(animBlueprintObjectFinder.Object);
	}
	// 메시 머티리얼 설정
	//MaterialInstanceConstant'/Game/PF/PFCharacter/Mannequins/Materials/MI_Manny_01_Black.MI_Manny_01_Black'
	//MaterialInstanceConstant'/Game/PF/PFCharacter/Mannequins/Materials/MI_Manny_02_Black.MI_Manny_02_Black'
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialBottomObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFCharacter/Mannequins/Materials/MI_Manny_01_Black.MI_Manny_01_Black'"));
	if (materialBottomObj.Succeeded())
	{
		dynamicMaterialBottom = GetMesh3P()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, materialBottomObj.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialTopObj(
		TEXT("MaterialInstanceConstant'/Game/PF/PFCharacter/Mannequins/Materials/MI_Manny_02_Black.MI_Manny_02_Black'"));
	if (materialTopObj.Succeeded())
	{
		dynamicMaterialTop = GetMesh3P()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, materialTopObj.Object);
	}

	// 무장 컴포넌트 생성
	UPFPistolWeapon* pistolWeapon = CreateDefaultSubobject<UPFPistolWeapon>(TEXT("PistolWeapon"));
	pistolWeapon->SetIsReplicated(true);
	pistolWeapon->ServerSetWeaponOwner(this);
	weapons.Insert(pistolWeapon, 0);
	UPFRifleWeapon* rifleWeapon = CreateDefaultSubobject<UPFRifleWeapon>(TEXT("RifleWeapon"));
	rifleWeapon->SetIsReplicated(true);
	rifleWeapon->ServerSetWeaponOwner(this);
	weapons.Insert(rifleWeapon, 1);
	UPFLasorCannonWeapon* lasorCannonWeapon = CreateDefaultSubobject<UPFLasorCannonWeapon>(TEXT("LasorCannonWeapon"));
	lasorCannonWeapon->SetIsReplicated(true);
	lasorCannonWeapon->ServerSetWeaponOwner(this);
	weapons.Insert(lasorCannonWeapon, 2);
	UPFGrenadeWeapon* grenadeWeapon = CreateDefaultSubobject<UPFGrenadeWeapon>(TEXT("GrenadeWeapon"));
	grenadeWeapon->SetIsReplicated(true);
	grenadeWeapon->ServerSetWeaponOwner(this);
	weapons.Insert(grenadeWeapon, 3);
	for (auto& w : weapons)
	{
		w->GetMesh3P()->SetIsReplicated(true);
	}

	// 무장 관련 변수 초기화
	holdingWeapon = nullptr;
	holdingWeaponIndex = -1;

	// 기본 무장 장탄 지급
	weapons[0]->SetRemainAmmo(60);
	// 기본 무장 선택
	SelectPistolWeapon();
}

void APFNormalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(InputComponent);
	Super::SetupPlayerInputComponent(InputComponent);
	// jump 입력을 일반 점프로 바인드
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APFCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APFCharacter::StopJumping);
	// 기본공격을 위하여 PrimaryAction을 오버라이딩하여 바인드
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APFNormalCharacter::OnPrimaryAction);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &APFNormalCharacter::AfterPrimaryAction);
	// 무장 선택을 위해 SelectOne - Four을 바인드
	PlayerInputComponent->BindAction("SelectOne", IE_Pressed, this, &APFNormalCharacter::SelectPistolWeapon); // 1번 무장 선택
	PlayerInputComponent->BindAction("SelectTwo", IE_Pressed, this, &APFNormalCharacter::SelectRifleWeapon); // 2번 무장 선택
	PlayerInputComponent->BindAction("SelectThree", IE_Pressed, this, &APFNormalCharacter::SelectLasorCannonWeapon); // 3번 무장 선택
	PlayerInputComponent->BindAction("SelectFour", IE_Pressed, this, &APFNormalCharacter::SelectGrenadeWeapon); // 4번 무장 선택
	// 무장 선택을 위해 Wheel을 바인드
	PlayerInputComponent->BindAxis("Wheel", this, &APFNormalCharacter::SelectNextWeapon);
	// 무장 장전을 위해 Reload를 바인드
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APFNormalCharacter::Reload);
	// 기타 상호작용을 위하여 Interaction을 바인드
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &APFNormalCharacter::SetOnInteraction);
	PlayerInputComponent->BindAction("Interaction", IE_Released, this, &APFNormalCharacter::SetOffInteraction);
}

void APFNormalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void APFNormalCharacter::BeginPlay()
{
	Super::BeginPlay();

}

float APFNormalCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
	AController* EventIntigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventIntigator, DamageCauser);
	if (GetLocalRole() == ROLE_Authority && NormalPlayerState != nullptr)
	{
		if (GetShield() > 0.0f)
		{
			// 실드에 피해, 초과 피해는 버림
			DecreaseShield(Damage);
		}
		else
		{
			// 체력에 피해
			DecreaseHealth(Damage);
			if (GetHealth() <= 0.0f)
			{
				// 체력이 0이 된 경우, 캐릭터 사망
				APFGameMode* gameMode = GetWorld()->GetAuthGameMode<APFGameMode>();
				if (gameMode->IsGameStart())
				{
					// 게임 진행중인 경우, 목숨을 1개 소모하고 리스폰
					DecreaseLife(1);
					bool canRespawn = false;
					if (GetLife() > 0) // 소모할 목숨이 없는 경우, 리스폰 불가능
					{
						canRespawn = true;
					}
					NormalKilled(canRespawn);
				}
				else
				{
					// 게임 진행중이 아닌 경우, 목숨을 소모하지 않고 리스폰
					NormalKilled(true);
				}
			}
		}
	}
	return Damage;
}

void APFNormalCharacter::OnPrimaryAction()
{
	Super::OnPrimaryAction();
	// 기본 공격 키 입력 시 실행
	if (holdingWeapon != nullptr)
	{
		TArray<FVector> mouseDirAndPos = GetMouseDirectionAndPosition();
		mouseDirAndPos[0].Normalize();
		holdingWeapon->OnFire(mouseDirAndPos[0], mouseDirAndPos[1]);
	}
}

void APFNormalCharacter::AfterPrimaryAction()
{
	if (holdingWeapon != nullptr)
	{
		TArray<FVector> mouseDirAndPos = GetMouseDirectionAndPosition();
		mouseDirAndPos[0].Normalize();
		holdingWeapon->AfterFire(mouseDirAndPos[0], mouseDirAndPos[1]);
	}
}

void APFNormalCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController->GetPlayerState<APlayerState>() != nullptr)
	{
		if (NewController->GetPlayerState<APlayerState>()->GetClass()->IsChildOf<APFNormalPlayerState>())
		{
			// PlayerState 포인터 지정
			SetNormalPlayerState(NewController->GetPlayerState<APFNormalPlayerState>());
			if (NormalPlayerState != nullptr)
			{
				// 캐릭터 메시 색상 설정
				FLinearColor color = NormalPlayerState->GetColor();
				ClientSetNormalColor(color);
				MulticastSetNormalColor(color);
			}
		}
	}
}


void APFNormalCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	// 월드 밖(KillZ 미만)으로 낙하할 때 실행
	// Super 함수(캐릭터를 소멸함)를 실행하지 않음
	//Super::FellOutOfWorld(dmgType);
	APFGameMode* gameMode = GetWorld()->GetAuthGameMode<APFGameMode>();
	if (!(NormalPlayerState->IsObserving()))
	{
		if (gameMode->IsGameStart())
		{
			DecreaseLife(1);
			bool canRespawn = false;
			if (GetLife() > 0)
			{
				canRespawn = true;
			}
			NormalKilled(canRespawn);
		}
		else
		{
			NormalKilled(true);
		}
	}
}

void APFNormalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(APFNormalCharacter, dynamicMaterialBottom);

}

void APFNormalCharacter::SelectNextWeapon(float value)
{
	// value가 음수인 경우, 휠을 내림 -> 다음 번호 무장 선택
	// value가 양수인 경우, 휠을 올림 -> 이전 번호 무장 선택
	if (value != 0.0f)
	{
		int32 index = holdingWeaponIndex;
		if (value < 0.0f)
		{
			// 휠을 내림 -> 번호 증가
			for (int32 i = 0; i < 4; ++i)
			{
				++index;
				if (index > 3) { index = index - 4; }
				if (!weapons[index]->IsAmmoEmpty())
				{
					SelectWeapon(index);
					break;
				}
			}
		}
		else if (value > 0.0f)
		{
			// 휠을 올림 -> 번호 감소
			for (int32 i = 0; i < 4; ++i)
			{
				--index;
				if (index < 0) { index = index + 4; }
				if (!weapons[index]->IsAmmoEmpty())
				{
					SelectWeapon(index);
					break;
				}
			}
		}
	}
	
}

void APFNormalCharacter::SelectWeapon(const int32& number)
{
	// 특정 번호의 무장 선택
	if (holdingWeaponIndex == number)
	{
		return;
	}
	if (holdingWeapon != nullptr)
	{
		if (holdingWeapon->GetSwapDelay() > 0.0f)
		{
			return;
		}
	}
	if (weapons[number]->IsAmmoEmpty())
	{
		return;
	}

	if (holdingWeapon != nullptr)
	{
		ServerUnpickWeapon(holdingWeaponIndex);
	}
	ServerPickWeapon(number);
}

void APFNormalCharacter::SelectPistolWeapon()
{
	SelectWeapon(0);
}

void APFNormalCharacter::SelectRifleWeapon()
{
	SelectWeapon(1);
}

void APFNormalCharacter::SelectLasorCannonWeapon()
{
	SelectWeapon(2);
}

void APFNormalCharacter::SelectGrenadeWeapon()
{
	SelectWeapon(3);
}

void APFNormalCharacter::ServerPickWeapon_Implementation(const int32& number)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	PickWeapon(number);
	weapons[number]->PlayEquipAnim();
}

void APFNormalCharacter::PickWeapon_Implementation(const int32& number)
{
	// number에 해당하는 무장 선택
	if (number >= 0 && number <= 3)
	{
		weapons[number]->PickThisWeapon();
		holdingWeapon = weapons[number];
		holdingWeaponIndex = number;
	}
}

void APFNormalCharacter::ServerUnpickWeapon_Implementation(const int32& number)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	UnpickWeapon(number);
}

void APFNormalCharacter::UnpickWeapon_Implementation(const int32& number)
{
	// number에 해당하는 무장 해제
	if (number >= 0 && number <= 3)
	{
		weapons[number]->UnpickThisWeapon();
		holdingWeapon = nullptr;
		holdingWeaponIndex = -1;
	}
}

void APFNormalCharacter::Reload()
{
	if (holdingWeapon != nullptr)
	{
		holdingWeapon->Reload();
	}
}

void APFNormalCharacter::NormalKilled(bool canRespawn)
{
	// 일반 캐릭터 사망 시 실행
	PlayRagdoll();
	DetachFromControllerPendingDestroy();
	if (canRespawn)
	{
		// 리스폰
		FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
		worldTimerManager.SetTimer(respawnTimerHandle, this, &APFNormalCharacter::NormalRespawn, respawnTime, false);
	}
	else
	{
		// 리스폰 불가능, 플레이 종료
		GetWorld()->GetAuthGameMode<APFGameMode>()->NormalFinished(NormalPlayerState);
		// 래그돌 제거
		FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
		worldTimerManager.SetTimer(respawnTimerHandle, this, &APFNormalCharacter::NormalRagdollDestroy, respawnTime, false);
	}
	// 관전 상태 돌입
	NormalPlayerState->Observing();

}

void APFNormalCharacter::NormalRespawn()
{
	// 일반 캐릭터룰 리스폰하고 자신을 소멸
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(respawnTimerHandle);
	APFGameMode* gameMode = Cast<APFGameMode>(GetWorld()->GetAuthGameMode());
	APlayerController* normalController = Cast<APlayerController>(NormalPlayerState->GetNormalController());
	gameMode->NormalRespawn(normalController);
	Destroy();
}

void APFNormalCharacter::NormalRagdollDestroy()
{
	// 일반 캐릭터를 리스폰하지 않고, 래그돌을 소멸
	FTimerManager& worldTimerManager = GetWorld()->GetTimerManager();
	worldTimerManager.ClearTimer(respawnTimerHandle);
	Destroy();
}

APFNormalPlayerState* APFNormalCharacter::GetNormalPlayerState()
{
	if (NormalPlayerState)
	{
		return NormalPlayerState;
	}
	else
	{
		NormalPlayerState = Cast<APFNormalPlayerState>(GetPlayerState());
		return NormalPlayerState;
	}
}

void APFNormalCharacter::SetNormalPlayerState(APFNormalPlayerState* newPS)
{
	if (newPS != nullptr)
	{
		NormalPlayerState = newPS;
	}
}

TArray<FVector> APFNormalCharacter::GetMouseDirectionAndPosition()
{
	// 플레이어의 마우스가 바라보는 방향과 위치 계산
	TArray<FVector> mouseDirAndPos;
	FVector mousePos, mouseDir;
	APlayerController* controller = Cast<APlayerController>(GetController());
	FVector2D screenPos = GEngine->GameViewport->Viewport->GetSizeXY();
	controller->DeprojectScreenPositionToWorld(screenPos.X / 2.0f, screenPos.Y / 2.0f, mousePos, mouseDir);
	mouseDirAndPos.Insert(mouseDir, 0);
	mouseDirAndPos.Insert(mousePos, 1);
	return mouseDirAndPos;
}

void APFNormalCharacter::SetNormalColor(const FLinearColor& color)
{
	// 캐릭터 메시, 총기 메시 모두 색상 변경
	if (dynamicMaterialBottom != nullptr && dynamicMaterialTop != nullptr)
	{
		dynamicMaterialBottom->SetVectorParameterValue("TeamColor", color);
		dynamicMaterialTop->SetVectorParameterValue("TeamColor", color);
	}
	for (auto& w : weapons)
	{
		w->SetWeaponColor(color);
	}
}

void APFNormalCharacter::MulticastSetNormalColor_Implementation(const FLinearColor& color)
{
	SetNormalColor(color);
}

void APFNormalCharacter::ClientSetNormalColor_Implementation(const FLinearColor& color)
{
	SetNormalColor(color);
}

FLinearColor APFNormalCharacter::GetNormalColor() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetColor();
	}
	return FLinearColor::Transparent;
}

void APFNormalCharacter::SetOnInteraction_Implementation()
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->SetOnInteraction();
	}
}
void APFNormalCharacter::SetOffInteraction_Implementation()
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->SetOffInteraction();
	}
}
bool APFNormalCharacter::IsOnInteraction()
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->IsOnInteraction();
	}
	return false;
}
void APFNormalCharacter::SetHealth_Implementation(const float& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->SetHealth(value);
	}
}
void APFNormalCharacter::DecreaseHealth_Implementation(const float& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->DecreaseHealth(value);
	}
}
void APFNormalCharacter::IncreaseHealth_Implementation(const float& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->IncreaseHealth(value);
	}
}
void APFNormalCharacter::SetShield_Implementation(const float& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->SetShield(value);
	}
}
void APFNormalCharacter::DecreaseShield_Implementation(const float& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->DecreaseShield(value);
	}
}
void APFNormalCharacter::IncreaseShield_Implementation(const float& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->IncreaseShield(value);
	}
}
void APFNormalCharacter::SetLife_Implementation(const uint8& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->SetLife(value);
	}
}
void APFNormalCharacter::DecreaseLife_Implementation(const uint8& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->DecreaseLife(value);
	}
}
void APFNormalCharacter::IncreaseLife_Implementation(const uint8& value)
{
	if (NormalPlayerState != nullptr)
	{
		NormalPlayerState->IncreaseLife(value);
	}
}

float APFNormalCharacter::GetHealth() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetHealth();
	}
	return -1.0f;
}
float APFNormalCharacter::GetMaxHealth() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetMaxHealth();
	}
	return -1.0f;
}
float APFNormalCharacter::GetShield() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetShield();
	}
	return -1.0f;
}
float APFNormalCharacter::GetMaxShield() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetMaxShield();
	}
	return -1.0f;
}
uint8 APFNormalCharacter::GetLife() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetLife();
	}
	return -1;
}
uint8 APFNormalCharacter::GetMaxLife() const
{
	if (NormalPlayerState != nullptr)
	{
		return NormalPlayerState->GetMaxLife();
	}
	return -1;
}