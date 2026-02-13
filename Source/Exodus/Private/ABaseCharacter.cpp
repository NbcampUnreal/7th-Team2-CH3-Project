// Copyright © 2026 비전공회담. All rights reserved.


// Copyright © 2026 비전공회담. All rights reserved.


#include "ABaseCharacter.h"
#include "AExodusPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MonsterBase.h"

AABaseCharacter::AABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetWorldLocation(FVector(0, 0, 70));
	SpringArmComp->SocketOffset = FVector(0, -50, 50);
	SpringArmComp->TargetArmLength = 310.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NomalSpeed = 360.f;
	SprintSpeedMultiplier = 1.75;
	SprintSpeed = NomalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
	
	//체력
	MaxHP = 10.0f;
	CurrentHP = MaxHP;
	
	//
	Attack = 20;
	//스테미나
	MaxStamina = 100;
	Stamina = MaxStamina;
	
	//총알
	MaxClip = 15;
	CurrentClip = MaxClip;
	CurrentReserveAmmo = 60;
	
	//상태
	bCanFire = true;
	bIsSprint = false;

}

void AABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	CurrentClip = MaxClip;


	if (WBP_CrossLine)
	{
		CrossLine = CreateWidget<UUserWidget>(GetWorld(), WBP_CrossLine);

		CrossLine->AddToViewport();
	}

}

// 데미지 처리                      //데미지 값, 데미지이벤트는 무슨공격에맞았냐? 이벤트 인스티게이터 상대Target , 맞은도구????
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
	FVector ShotDirection = FMath::VRandCone(ViewRotation.Vector(), FMath::DegreesToRadians(5.0f));
	FVector EndPos = ViewLocation + (ShotDirection * 5000.f);


	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		ViewLocation,
		EndPos,
		ECC_Visibility,
		Params);

	FColor LineColor = bIsHit ? FColor::Green : FColor::Red;

	DrawDebugLine(
		GetWorld(),
		ViewLocation,
		EndPos,
		LineColor,
		false,
		2.0f,
		0,
		3.0f
	);

	if (Hit.bBlockingHit)
	{
		AActor* hitAttcor = Hit.GetActor();
		if (hitAttcor)
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(hitAttcor);
			if (Monster)
			{
				Monster->Destroy();
				//Monster->GetHp()-Attak
			}
		}
	}
}

void AABaseCharacter::StartFire(const FInputActionValue& Value)
{
	if (!bCanFire || CurrentClip <= 0)
	{
		if (CurrentClip <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("탄약 부족! 재장전(R)이 필요합니다."));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("탄약부족  남은탄약: % d / % d"), CurrentClip, MaxClip));
		}
		return;
	}
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
	Params.AddIgnoredActor(this);
	CurrentClip--;
	bCanFire = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("발사! 남은탄약: %d/%d"), CurrentClip, MaxClip));

	for (int i = 0; i < 4; ++i)
	{
		AABaseCharacter::Fire();
	}
	float FireRate = 0.5;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AABaseCharacter::ResultFire, FireRate, false);
}
// 재장전
void AABaseCharacter::Reload()
{
	if (CurrentClip >= MaxClip || CurrentReserveAmmo <= 0) return;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,FString::Printf(TEXT("재장전중 %d/%d"), CurrentClip, MaxClip));

	bCanFire = false;
	float ReloadTime = 1.5f;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AABaseCharacter::CompleteReload, ReloadTime, false);
}

void AABaseCharacter::CompleteReload()
{
	int32 AmmoNeeded = MaxClip - CurrentClip;
	int32 AmmoToLoad = FMath::Min(AmmoNeeded, CurrentReserveAmmo);

	CurrentClip += AmmoToLoad;
	CurrentReserveAmmo -= AmmoToLoad;

	bCanFire = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,FString::Printf(TEXT("재장전완료! 현재 탄약:%d/%d"), CurrentClip, MaxClip));
}

// Called every frame
void AABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bIsSprint)
	{
		Stamina = FMath::Min(Stamina+(10.0f*DeltaTime),MaxStamina);
	}
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
					ETriggerEvent::Started,
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

			if (PlayerController->FireAction)
			{
				EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::StartFire


				);
			}
			if (PlayerController->ReloadAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ReloadAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::Reload
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

//스프린트 액션 

void AABaseCharacter::StartSpirnt(const FInputActionValue& Value)
{
	if (bIsSprint || Stamina <= 0) return;
	
	if (GetCharacterMovement())
	{
		bIsSprint = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &AABaseCharacter::UpdateStamina, 0.1f,true);
	}
}

void AABaseCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		bIsSprint = false;
		GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
		GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
	}
}

void AABaseCharacter::UpdateStamina()
{
	Stamina -= 1.f;
	if(Stamina <= 0)
	{
		Stamina = 0;
		StopSprint(FInputActionValue());
	}
}

void AABaseCharacter::ResultFire()
{
	bCanFire = true;
}

//void UpdateHp()
//{
//	if()
//}