// Copyright © 2026 비전공회담. All rights reserved.


// Copyright © 2026 비전공회담. All rights reserved.


#include "ABaseCharacter.h"
#include "AExodusPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
AABaseCharacter::AABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetWorldLocation(FVector(0, 0, 70));
	SpringArmComp->SocketOffset = FVector(0, 0, 50);
	SpringArmComp->TargetArmLength = 310.f;
	SpringArmComp->bUsePawnControlRotation = true;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NomalSpeed = 360.f;
	SprintSpeedMultiplier = 1.75;
	SprintSpeed = NomalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;

	MaxHP = 10.0f;
	CurrentHP = MaxHP;

	MaxClip = 15;
	CurrentClip = MaxClip;
	CurrentReserveAmmo = 30;
}

// Called when the game starts or when spawned
void AABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작시 초기 스탯 재설정
	CurrentHP = MaxHP;
	CurrentClip = MaxClip;
}

// 데미지 처리
float AABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.0f) return 0.0f;

	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("피격! 남은 체력: %.1f"), CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("캐릭터 사망!"));
	}

	return ActualDamage;
}

// 사격
void AABaseCharacter::Fire()
{
	if (CurrentClip > 0)
	{
		CurrentClip--;
		UE_LOG(LogTemp, Log, TEXT("발사! 남은 탄약: %d / %d"), CurrentClip, CurrentReserveAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("탄약 부족! 재장전(R)이 필요합니다."));
	}
}

// 재장전
void AABaseCharacter::Reload()
{
	if (CurrentClip >= MaxClip || CurrentReserveAmmo <= 0) return;

	int32 AmmoNeeded = MaxClip - CurrentClip;
	int32 AmmoToLoad = FMath::Min(AmmoNeeded, CurrentReserveAmmo);

	CurrentClip += AmmoToLoad;
	CurrentReserveAmmo -= AmmoToLoad;

	UE_LOG(LogTemp, Log, TEXT("재장전 완료! 현재 탄약: %d / %d"), CurrentClip, CurrentReserveAmmo);
}

// Called every frame
void AABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AAExodusPlayerController* PlayerController = Cast<AAExodusPlayerController>(GetController()))
		{

			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AABaseCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::Look
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::StartSpirnt
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AABaseCharacter::StopSprint
				);
			}


		}
	}
}

void AABaseCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) { return; }

	const FVector2D MoveInput = Value.Get<FVector2D>();
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AABaseCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void AABaseCharacter::StopJump(const FInputActionValue& Value)
{

	if (Value.Get<bool>())
	{
		StopJumping();
	}
}

void AABaseCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AABaseCharacter::StartSpirnt(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AABaseCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
	}
}