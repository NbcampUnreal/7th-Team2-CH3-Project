// Copyright © 2026 비전공회담. All rights reserved.


// Copyright © 2026 비전공회담. All rights reserved.


#include "ABaseCharacter.h"
#include "AExodusPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MonsterBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AABaseCharacter::AABaseCharacter()
{
	//틱함수 사용여부
	PrimaryActorTick.bCanEverTick = true;

	//스프링암 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetWorldLocation(FVector(0, 0, 70));
	SpringArmComp->SocketOffset = FVector(0, -50, 50);
	SpringArmComp->TargetArmLength = 310.f;
	SpringArmComp->bUsePawnControlRotation = true;

	// 카메라 설정	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	//이동속도
	NomalSpeed = 360.f;
	SprintSpeedMultiplier = 2.0f;
	SprintSpeed = NomalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;

	//체력
	MaxHP = 10.0f;
	CurrentHP = MaxHP;

	//공격력	
	Attack = 20;

	//스테미나	
	MaxStamina = 100;
	Stamina = MaxStamina;

	//총알	
	MaxClip = 15;
	CurrentClip = MaxClip;
	CurrentReserveAmmo = 60;

	//상태	
	bIsReloading = false;
	bCanFire = true;
	bIsSprint = false;
	bCanLaunch = true;
	bIsStealthMode = false;

	//수류탄	
	GrenadeClass = nullptr;
	GrenadeCount = 5;

	//총알이펙트및 총구이펙트 소켓 이름들
	MuzzleSocketNames.Add(TEXT("gun_pinSocket"));
	MuzzleSocketNames.Add(TEXT("gun_pinSocket_0"));
	MuzzleSocketNames.Add(TEXT("gun_pinSocket_1"));
	MuzzleSocketNames.Add(TEXT("gun_pinSocket_2"));

	CurrentOverlappedItem = nullptr;
	// 죽음
	bIsdie = false;
	bIsDead = false;
}


bool AABaseCharacter::AddItemToInventory(AActor* NewItem)
{
	//아이템이 실제로 존재하지 않거나  인벤토리 내용물이 10보다크다면 리턴
	if (NewItem == nullptr || Inventory.Num() >= MaxInventorySlots)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("인벤토리가 가득 찼거나 잘못된 아이템입니다."));
		return false;
	}
	// 인벤토리에 아이템 추가
	Inventory.Add(NewItem);
	// 엑터를 게임에서 숨긴다? (인벤토리에 들어갔다고 보이게하는건가)
	NewItem->SetActorHiddenInGame(true);
	// 엑터콜리전 설정 모든 충돌삭제 그럼바닥통과해버리는거아님?
	NewItem->SetActorEnableCollision(false);
	// 내캐릭터몸에 붙여라?
	NewItem->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%s 획득 완료!"), *NewItem->GetName()));
	
	return true;
}

// 이벤트 시작시
//체력과 총알을 초기화해주고
void AABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	CurrentClip = MaxClip;
	if (GetMesh())
	{
		// 8번은 확실히 추가하고, 3번(Gun)과 6번(Glow)도 같이 넣어두면 안전합니다!
		WeaponMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(3));
		//WeaponMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(6));
		//WeaponMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(8));
	}
	//머테리얼 갯수 가져오기
	int32 MaterialCount = GetMesh()->GetNumMaterials();
	// 
	for (int i = 0; i < MaterialCount; ++i)
	{
		UMaterialInstanceDynamic* DynMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);

		if (DynMaterial)
		{
			CharacterMaterials.Add(DynMaterial);
			DynMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
		}
	}


	if (WBP_CrossLine)
	{
		CrossLine = CreateWidget<UUserWidget>(GetWorld(), WBP_CrossLine);

		CrossLine->AddToViewport();
	}
}

// 데미지 처리                      //데미지 값, 데미지이벤트는 무슨공격에맞았냐? 이벤트 인스티게이터 상대Target , 맞은도구????
float AABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("TakeDamage 함수 진입! 들어온 데미지: %f"), DamageAmount);

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("데미지가 0이거나 음수로 들어왔습니다!"));
		return 0.0f;
	}
	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("데미지 적용 성공! 남은 HP: %f"), CurrentHP);
	if (CurrentHP <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("캐릭터 사망!"));
	}
	return ActualDamage;
}

// 사격
void AABaseCharacter::Fire(int32 SocketIndex)
{
	FName TargetSocketName = MuzzleSocketNames[SocketIndex];

	// 소켓 위치 가져옴
	if (!MuzzleSocketNames.IsValidIndex(SocketIndex)) return;
	FName TargetSocket = MuzzleSocketNames[SocketIndex];
	FVector MuzzleLocation = GetMesh()->GetSocketLocation(TargetSocket);


	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(
			MuzzleEffect,
			GetMesh(),
			TargetSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset
		);
	}


	if (FireMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("몽타주 재생 시작!"));
		PlayAnimMontage(FireMontage);
	}

	FVector ShotDirection = FMath::VRandCone(ViewRotation.Vector(), FMath::DegreesToRadians(5.0f));
	FVector EndPos = ViewLocation + (ShotDirection * 5000.f);

	if (bIsStealthMode)
	{
		Stealth(true);
	}
	// 카메라에서 나가는 라인트레이서
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		ViewLocation,
		EndPos,
		ECC_Visibility,
		Params);

	//FColor LineColor = bIsHit ? FColor::Green : FColor::Red;
	FVector BeamEnd = bIsHit ? Hit.ImpactPoint : EndPos;

	// DrawDebugLine(
	// 	GetWorld(),
	// 	ViewLocation,
	// 	EndPos,
	// 	LineColor,
	// 	false,
	// 	2.0f,
	// 	0,
	// 	3.0f
	// );

	// 이펙트가 보이게하는 함수
	if (TrailEffect)
	{
		//
		FVector ToTarget = (BeamEnd - MuzzleLocation);
		UParticleSystemComponent* TrailComp = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			TrailEffect,
			MuzzleLocation,
			ToTarget.Rotation()
		);

		if (TrailComp)
		{
			float speed = 7000.f;
			FVector LaunchVelocity = ToTarget.GetSafeNormal() * speed;
			float Distance = ToTarget.Size();
			
			TrailComp->Activate(true);

			TrailComp->SetVectorParameter(FName("Velocity"), LaunchVelocity);
			TrailComp->bAutoDestroy = true;

			float TimeToHit = Distance / speed;
			
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [TrailComp]()
			{
				if (TrailComp && TrailComp->IsValidLowLevel())
				{
					TrailComp->DestroyComponent();
				}
			}, TimeToHit, false);
		}
	}
	if (Hit.bBlockingHit)
	{
		AActor* hitAttcor = Hit.GetActor();
		if (hitAttcor)
		{
			AMonsterBase* Monster = Cast<AMonsterBase>(hitAttcor);
			if (Monster)
			{
				Monster->ReceiveDamage(20);
				UE_LOG(LogTemp, Warning, TEXT("몬스터 체력 깎음! 현재 HP: %d"), Monster->GetHp());
			}
		}
	}
}

void AABaseCharacter::StartFire(const FInputActionValue& Value)
{
	if (!bCanFire) { return; }
	bCanFire = false;
	float FireRate = 0.7;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AABaseCharacter::ResultFire, FireRate, false);
	if (CurrentClip <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("탄약부족 재장전 (R)이 필요합니다. 남은 탄약:%d/%d"), CurrentClip, MaxClip));
		return;
	}

	CurrentClip--;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

	Params.AddIgnoredActor(this);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
	                                 FString::Printf(TEXT("발사! 남은탄약: %d/%d"), CurrentClip, MaxClip));

	for (int i = 0; i < 4; ++i)
	{
		Fire(i);
	}
}

// 재장전
void AABaseCharacter::Reload()
{
	if (bIsReloading)return;

	bool bIsFull = CurrentClip >= MaxClip;
	bool bNoAmmo = CurrentReserveAmmo <= 0;

	if (bIsFull || bNoAmmo)
	{
		FString FailMsg = bIsFull ? TEXT("탄창이 가득 찼습니다") : TEXT("총알이 없습니다");
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FailMsg);

		bIsReloading = true;
		bCanFire = false;

		float WaitTime = 1.0f;
		GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AABaseCharacter::CompleteReload, WaitTime, false);
		return;
	}

	if (ReloadMontage)
	{
		float fastanim = 1.6f;
		UE_LOG(LogTemp, Warning, TEXT("몽타주 재생 시작!"));
		PlayAnimMontage(ReloadMontage, fastanim);
	}

	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("재장전중 %d/%d"), CurrentClip, MaxClip));

	bIsReloading = true;
	bCanFire = false;
	float ReloadTime = 5.f;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AABaseCharacter::CompleteReload, ReloadTime, false);
	GetCharacterMovement()->MaxWalkSpeed = NomalSpeed * 0.7;
}


void AABaseCharacter::CompleteReload()
{
	int32 AmmoNeeded = MaxClip - CurrentClip;
	int32 AmmoToLoad = FMath::Min(AmmoNeeded, CurrentReserveAmmo);

	CurrentClip += AmmoToLoad;
	CurrentReserveAmmo -= AmmoToLoad;

	bIsReloading = false;
	bCanFire = true;
	GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;;
}

void AABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsSprint && !bIsStealthMode)
	{
		Stamina = FMath::Min(Stamina + (10.0f * DeltaTime), MaxStamina);
	}
	if (bIsDead)
	{
		Die();	
	}
	if (bIsStealthMode)
	{
		Stamina -= 10 * DeltaTime;
		if (Stamina < 0.0f)
		{
			Stamina = 0.0f;
			Stealth(true);
		}
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
					&AABaseCharacter::StartSprint
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

			if (PlayerController->ThrowGrenadeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ThrowGrenadeAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::LaunchGrenade
				);
			}

			if (PlayerController->StealthAction)
			{
				EnhancedInput->BindAction(
					PlayerController->StealthAction,
					ETriggerEvent::Triggered,
					this,
					&AABaseCharacter::Stealth,
					false
				);
			}
			if (PlayerController->GetItemAction)
			{
				EnhancedInput->BindAction(
					PlayerController->GetItemAction,
					ETriggerEvent::Started,
					this,
					&AABaseCharacter::OnItemActionPressed
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

void AABaseCharacter::StartSprint(const FInputActionValue& Value)
{
	if (bIsSprint || Stamina <= 0) return;

	if (GetCharacterMovement())
	{
		bIsSprint = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &AABaseCharacter::UpdateStamina, 0.1f, true);
		if (bIsReloading)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * 0.7;
		}
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
	if (Stamina <= 0)
	{
		Stamina = 0;
		StopSprint(FInputActionValue());
	}
}


void AABaseCharacter::ResultFire()
{
	bCanFire = true;
}

void AABaseCharacter::LaunchGrenade()
{
	//수류탄을 던진상태인가? 맞다면 리턴
	if (!bCanLaunch) { return; }

	// 던지고나면 FALSE로 수정
	if (GrenadeClass && GrenadeCount > 0)
	{
		if (GrenadeMontage)
		{
			float fastanim = 1.6f;
			PlayAnimMontage(GrenadeMontage, fastanim);
			UE_LOG(LogTemp, Warning, TEXT("몽타주 재생 시작!"));
		}
		bCanLaunch = false;

		GetWorld()->GetTimerManager().SetTimer(
			LaunchTimerHandle,
			this,
			&AABaseCharacter::RealLaunch,
			1.0f,
			false
		);
		SetWeaponOpacity(0);
	}
}

void AABaseCharacter::RealLaunch()
{
	//수류탄이있다면
	if (GrenadeClass && GrenadeCount > 0)
	{
		// 스폰 로케이션에 메쉬의 스켈레톤 메쉬의 hand_r 에있는 위치르 를가져오고
		FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
		// 스폰 로테이터 생성시의 회전값을 가져온다
		FRotator SpawnRotation = GetControlRotation();
		//액터를 스폰할 명령어들
		FActorSpawnParameters SpawnParams;
		//파라마미터에 있는 오너를 자기자신(캐릭터)으로 설정하고
		SpawnParams.Owner = this;
		// 이 수류탄을 누가 던질것인가?
		SpawnParams.Instigator = GetInstigator();
		//스폰시 충돌이 일어나면 어떠한방법을 사용할것인가  충돌을무시하고 생성
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		// 수류탄타입인 변수하나 만들고 수류탄 생성
		AGrenade* SpwanedGrenade = GetWorld()->SpawnActor<AGrenade>(GrenadeClass, SpawnLocation, SpawnRotation,
		                                                            SpawnParams);
		// 수류탄이 생성됬다면
		if (SpwanedGrenade)
		{
			GrenadeCount--;

			//수류탄의 물리엔진을가져온다
			UProjectileMovementComponent* ProjectileComp = SpwanedGrenade->GetProjectileMovement();
			//루트 컴포넌트를 조작하기위한 형변환
			UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SpwanedGrenade->GetRootComponent());

			// 루트 프라밋티비가 있다면
			if (RootPrimitive)
			{
				// 이그노어 설정을 즉 무시할 애들을 나자신(캐릭터와)을 진짜 무시할거야 true
				RootPrimitive->IgnoreActorWhenMoving(this, true);
			}
			GetWorld()->GetTimerManager().SetTimer(
				LaunchTimerHandle,
				this,
				&AABaseCharacter::bLaunch,
				5.0f,
				false);
			// 프로젝트컴프가 존재한다면
			if (ProjectileComp)
			{
				// 물리엔진 초기화
				ProjectileComp->StopMovementImmediately();

				//변수 생성
				FVector CameraLoc;
				FRotator CameraRot;
				//카메라 위치 담는코드
				GetController()->GetPlayerViewPoint(CameraLoc, CameraRot);
				// 라인트레이서함수에 사용할 변수들
				FHitResult GrenadeHit;
				FVector Start = CameraLoc;
				FVector End = Start + (CameraRot.Vector() * 10000.f);
				FCollisionQueryParams TraceParams;
				TraceParams.AddIgnoredActor(this);

				FVector TargetLocation;
				//라인트레이서가 성공했다면
				if (GetWorld()->LineTraceSingleByChannel(GrenadeHit, Start, End, ECC_Visibility, TraceParams))
				{
					// 수류탄이 부딪힌 지점
					TargetLocation = GrenadeHit.ImpactPoint;
				}
				else
				{
					//그냥끝까지만감
					TargetLocation = End;
				}

				//위에 라인트레이서 결과값에서 엑터 스폰위치를빼면 방향이나옴?
				FVector LaunchDir = (TargetLocation - SpawnLocation).GetSafeNormal();
				//방향 계산
				LaunchDir += FVector(0.f, 0.f, 0.3f);
				LaunchDir.Normalize();

				//던지는 속도
				float ThrowSpeed = 1500.f;


				ProjectileComp->InitialSpeed = ThrowSpeed;
				ProjectileComp->MaxSpeed = ThrowSpeed;
				// 방향하고 속도
				ProjectileComp->Velocity = LaunchDir * ThrowSpeed;

				// 강제로 벡터값 업데이트
				ProjectileComp->UpdateComponentVelocity();
				//틱함수 활성화해서 틱마다 수류탄 위치 업데이트
				ProjectileComp->SetComponentTickEnabled(true);
				// 컴포넌트활성화해서 날라가게함
				ProjectileComp->Activate(true);


				if (RootPrimitive)
				{
					//물리시뮬레이션끔
					RootPrimitive->SetSimulatePhysics(false);
					// 자기자신과의 충돌무시
					RootPrimitive->IgnoreActorWhenMoving(this, true);
				}
			}
		}
		SetWeaponOpacity1(1);
		if (bIsStealthMode)
		{
			Stealth(true);
		}
	}
}


void AABaseCharacter::bLaunch()
{
	bCanLaunch = true;
}

void AABaseCharacter::Stealth(bool bIsForce)
{
	//은신상태가 아니면서 스테미나가 0보다 크면 
	if (!bIsForce && bIsStealthCooldown)
	{
		return;
	}

	if (!bIsStealthMode && Stamina > 0.0f)
	{
		//은신상태로
		bIsStealthMode = true;
		Tags.Remove(TEXT("Player"));
		Tags.AddUnique(TEXT("Stealth"));
		for (UMaterialInstanceDynamic* Mat : CharacterMaterials)
		{
			if (Mat)
			{
				Mat->SetScalarParameterValue(TEXT("Opacity"), 0.2f);
			}
		}
		GEngine->AddOnScreenDebugMessage(-1,
		                                 2.0f,
		                                 FColor::Cyan,
		                                 TEXT("은신 활성화!"));
	}
	else
	{
		bIsStealthMode = false;
		Tags.Remove(TEXT("Stealth"));
		Tags.AddUnique(TEXT("Player"));
		for (UMaterialInstanceDynamic* Mat : CharacterMaterials)
		{
			if (Mat)
			{
				Mat->SetScalarParameterValue(TEXT("Opacity"), 1.0f); // 원래대로 복구 [cite: 2026-02-18]
			}
		}
		GEngine->AddOnScreenDebugMessage(-1,
		                                 2.0f,
		                                 FColor::Yellow,
		                                 TEXT("은신 해제!"));
	}
	bIsStealthCooldown = true;

	GetWorldTimerManager().SetTimer(
		StealthTimerHandle,
		this,
		&AABaseCharacter::StealthCoolDown,
		3.0f,
		false);
}

void AABaseCharacter::OnItemActionPressed(const FInputActionValue& Value)
{
	if (CurrentOverlappedItem != nullptr)
	{
		bool bSuccess = AddItemToInventory(CurrentOverlappedItem);
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("아이템 획득 성공!"));	
		}
	}
}

void AABaseCharacter::StealthCoolDown()
{
	bIsStealthCooldown = false;
}

void AABaseCharacter::SetWeaponOpacity(float NewOpacity)
{
	for (auto Mat : WeaponMaterials)
	{
		if (Mat)
		{
			// 머테리얼 에디터 내부의 파라미터 이름을 정확히 넣으세요!
			Mat->SetScalarParameterValue(TEXT("opacity"), NewOpacity);
		}
	}
}

void AABaseCharacter::SetWeaponOpacity1(float NewOpacity)
{
	for (auto Mat : WeaponMaterials)
	{
		if (Mat)
		{
			// 머테리얼 에디터 내부의 파라미터 이름을 정확히 넣으세요!
			Mat->SetScalarParameterValue(TEXT("opacity"), NewOpacity);
		}
	}
}

void AABaseCharacter::Die()
{
	GetController()->StopMovement();
	DisableInput(Cast<APlayerController>(GetController())); 


	GetCharacterMovement()->DisableMovement();
	if (DieMontage)
	{
		PlayAnimMontage(DieMontage);
	}
}

void AABaseCharacter::ReceiveDamage(int32 DamageAmount)
{
	CurrentHP -= DamageAmount;
	
	if (CurrentHP <= 0.f)
	{
		CurrentHP = 0.f;
		Die();
	}
}
//void hit();

//void UpdateHp()
//{	
//	if()
//}
