// Copyright © 2026 비전공회담. All rights reserved.


// Copyright © 2026 비전공회담. All rights reserved.


#include "ABaseCharacter.h"
#include "AExodusPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
//#include "DrawDebugHelpers.h"
#include "UMG.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "MonsterBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ExodusGameInstance.h"


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
	MaxHP = 100.0f;
	CurrentHP = MaxHP;

	//공격력	
	Attack = 100;

	//스테미나	
	MaxStamina = 100;
	Stamina = MaxStamina;

	//총알	
	MaxClip = 15;
	CurrentClip = MaxClip;
	CurrentReserveAmmo = 300;

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


		FString MapName = GetWorld()->GetMapName();
		MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		UExodusGameInstance* GI = Cast<UExodusGameInstance>(GetGameInstance());
		if (GI && MapName.Equals(TEXT("TestLevel22"), ESearchCase::IgnoreCase))
		{
			if (GI && MapName.Equals(TEXT("TestLevel22"), ESearchCase::IgnoreCase))
			{
				KillCount = GI->SaveKillCount;
				CurrentHP = GI->SaveCurrentHp;
				CurrentClip = GI->SaveCurrentClip;
				GrenadeAddCount = GI->SaveGrenadeAddCount;
				GrenadeCount = GI->SaveCurrentGrenade;
				CurrentReserveAmmo = GI->SaveCurrentReserveAmmo;

				if (KillCount >= 100) { MaxClip = 30; }
				else { MaxClip = 15; }
				
			}
		}

		if (GEngine)
		{
			UGameUserSettings* Settings = GEngine->GetGameUserSettings();
			if (Settings)
			{
				Settings->SetFullscreenMode(EWindowMode::Windowed);
				Settings->SetScreenResolution(FIntPoint(1280, 720));
				Settings->ApplySettings(true);
			}
		}


		if (GetMesh())
		{
			WeaponMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(3));
		}

		int32 MaterialCount = GetMesh()->GetNumMaterials();

		for (int i = 0; i < MaterialCount; ++i)
		{
			UMaterialInstanceDynamic* DynMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);

			if (DynMaterial)
			{
				CharacterMaterials.Add(DynMaterial);
				DynMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
			}
		}

		if (WBP_CrossLine) // 설계도 확인
		{
			// 실제 물건(CrossLine) 만들기
			CrossLine = CreateWidget<UUserWidget>(GetWorld(), WBP_CrossLine);
			if (CrossLine) CrossLine->AddToViewport();
		}
	}

void AABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}
}

float AABaseCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser);

	if (ActualDamage <= 0.f)
		return 0.f;

	// ====== HP 감소 ======
	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);

	if (GEngine)
	{
		FString HealthMessage = FString::Printf(
			TEXT("HP: %.1f / %.1f (입은 데미지: %.1f)"),
			CurrentHP,
			MaxHP,
			ActualDamage);

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, HealthMessage);
	}

	if (CurrentHP <= 0.f)
	{
		Die();
		return ActualDamage;
	}

	// ====== 피격 방향 계산 ======
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();
	FVector ToSource = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	float ForwardDot = FVector::DotProduct(Forward, ToSource);
	float RightDot = FVector::DotProduct(Right, ToSource);

	UAnimMontage* MontageToPlay = nullptr;

	if (ForwardDot > 0.5f)
	{
		MontageToPlay = HitFrontMontage;
	}
	else if (ForwardDot < -0.5f)
	{
		MontageToPlay = HitBackMontage;
	}
	else if (RightDot > 0.f)
	{
		MontageToPlay = HitRightMontage;
	}
	else
	{
		MontageToPlay = HitLeftMontage;
	}

	if (MontageToPlay && HitEffectSystem)
	{
		PlayAnimMontage(MontageToPlay);
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = (const FPointDamageEvent*)&DamageEvent;
		FHitResult Hitinfo = PointDamageEvent->HitInfo;

		FName HitBoneName = Hitinfo.MyBoneName;

		PlayHitEffect(Hitinfo);
	}
	return ActualDamage;
}

void AABaseCharacter::PlayHitEffect(const FHitResult& Hitd)
{
	if (HitEffectSystem && GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffectSystem,
			Hitd.ImpactPoint,
			Hitd.ImpactNormal.Rotation()
		);
	}
}

// 사격
void AABaseCharacter::Fire(int32 SocketIndex)
{
	FName TargetSocketName = MuzzleSocketNames[SocketIndex];
	FCollisionQueryParams Params1;
	// 소켓 위치 가져옴
	if (!MuzzleSocketNames.IsValidIndex(SocketIndex)) return;
	FName TargetSocket = MuzzleSocketNames[SocketIndex];
	FVector MuzzleLocation = GetMesh()->GetSocketLocation(TargetSocket);

	FVector MuzzleSoundLocation = GetMesh()->GetSocketLocation(TEXT("Muzzle_Socket_01"));
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleSoundLocation);
	}
	if (MuzzleEffect)
	{
		UParticleSystemComponent* MuzzleComp = UGameplayStatics::SpawnEmitterAttached(
			MuzzleEffect, GetMesh(), TargetSocketName,
			FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale
		);
		if (MuzzleComp)
		{
			FTimerHandle MuzzleTimerHandle;
			TWeakObjectPtr<UParticleSystemComponent> WeakMuzzle = MuzzleComp;

			GetWorldTimerManager().SetTimer(MuzzleTimerHandle, [WeakMuzzle]()
			{
				if (UParticleSystemComponent* StrongMuzzle = WeakMuzzle.Get())
				{
					if (StrongMuzzle->IsValidLowLevel())
					{
						StrongMuzzle->DestroyComponent();
					}
				}
			}, 0.2f, false);
		}
	}

	FTimerHandle FireDelayHandle;
	GetWorldTimerManager().SetTimer(FireDelayHandle, [WeakThis = TWeakObjectPtr<AABaseCharacter>(this)]()
	{
		AABaseCharacter* StrongThis = WeakThis.Get();
		if (StrongThis && StrongThis->FireMontage)
		{
			StrongThis->PlayAnimMontage(StrongThis->FireMontage, 1.0f, NAME_None);
		}
	}, 0.05f, false);

	FVector ShotDirection = FMath::VRandCone(ViewRotation.Vector(), FMath::DegreesToRadians(2.0f));
	//FVector ShotDirection = ViewRotation.Vector();
	FVector EndPos = ViewLocation + (ShotDirection * 5000.f);

	if (bIsStealthMode)
	{
		Stealth(true);
	}
	Params1.bTraceComplex = true; // 복잡한 충돌(뼈) 체크
	Params1.bReturnFaceIndex = true; // 뼈 이름 반환 유도
	Params1.AddIgnoredActor(this);


	// 카메라에서 나가는 라인트레이서
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		ViewLocation,
		EndPos,
		ECC_Visibility,
		Params1);

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
			TWeakObjectPtr<UParticleSystemComponent> WeakTrail = TrailComp;
			GetWorldTimerManager().SetTimer(TimerHandle, [WeakTrail]()
			{
				if (UParticleSystemComponent* StrongTrail = WeakTrail.Get())
				{
					if (StrongTrail->IsValidLowLevel())
					{
						StrongTrail->DestroyComponent();
					}
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
				float FinalDamage = Attack;
				FName HitBone = Hit.BoneName;
				UE_LOG(LogTemp, Warning, TEXT("지금 내가 맞춘 뼈 이름: %s"), *HitBone.ToString());
				if (HitBone == TEXT("head"))
				{
					FinalDamage = Attack * 2;
					UE_LOG(LogTemp, Warning, TEXT(" 헤드샷 몬스터 체력 깎음! 현재 HP: %d"), Monster->GetHp());
				}
				Monster->ReceiveDamage(FinalDamage);
				UE_LOG(LogTemp, Warning, TEXT("몬스터 체력 깎음! 현재 HP: %d"), Monster->GetHp());
				if (CrossLine)
				{
					FProperty* AnimProp = CrossLine->GetClass()->FindPropertyByName(FName("HitAnim"));
					if (FObjectProperty* ObjectProp = CastField<FObjectProperty>(AnimProp))
					{
						UWidgetAnimation* FoundAnim = Cast<UWidgetAnimation>(
							ObjectProp->GetObjectPropertyValue_InContainer(CrossLine));

						if (FoundAnim)
						{
							CrossLine->PlayAnimation(FoundAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
						}
					}
				}
			}
		}
	}
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		RecoilRemainingPitch += FMath::FRandRange(-0.5f, -1.2f);
		RecoilRemainingYaw += FMath::FRandRange(-0.2f, 0.2f);
	}
}

void AABaseCharacter::StartFire(const FInputActionValue& Value)
{
	if (bIsReloading)
	{
		StopAnimMontage(ReloadMontage);
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		if (ReloadTimerHandles.Num() > 0)
		{
			for (FTimerHandle& Handle : ReloadTimerHandles)
			{
				GetWorldTimerManager().ClearTimer(Handle);
			}
			ReloadTimerHandles.Empty();
		}
		bCanFire = true;
		bIsReloading = false;
		GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
	}

	if (!bCanFire) { return; }
	float FireRate = 0.7;

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AABaseCharacter::ResultFire, FireRate, false);
	if (CurrentClip <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("탄약부족 재장전 (R)이 필요합니다. 남은 탄약:%d/%d"), CurrentClip, MaxClip));
		return;
	}

	bCanFire = false;
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

	bIsReloading = true;
	bCanFire = false;
	Stamina -= 15;
	bool bIsFull = CurrentClip >= MaxClip;
	bool bNoAmmo = CurrentReserveAmmo <= 0;

	if (bIsFull || bNoAmmo)
	{
		FString FailMsg = bIsFull ? TEXT("탄창이 가득 찼습니다") : TEXT("총알이 없습니다");
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FailMsg);

		bIsReloading = true;
		bCanFire = false;

		return;
	}

	if (ReloadMontage)
	{
		float fastanim = 1.6f;
		UE_LOG(LogTemp, Warning, TEXT("몽타주 재생 시작!"));
		PlayAnimMontage(ReloadMontage, fastanim);
		if (ReloadingSound && ReloadOpenMiddleSound)
		{
			FTimerHandle ReloadOpenTimer;
			FTimerHandle ReloadEndTimer;


			GetWorldTimerManager().SetTimer(ReloadOpenTimer, [this]()
			{
				UGameplayStatics::PlaySoundAtLocation(this, ReloadOpenMiddleSound,
				                                      GetMesh()->GetSocketLocation(TEXT("Muzzle_Socket_01")));
				UE_LOG(LogTemp, Warning, TEXT("1.5초 사운드 재생 성공"));
			}, 0.9f, false);
			ReloadTimerHandles.Add(ReloadOpenTimer);

			for (int32 i = 0; i < 5; ++i)
			{
				FTimerHandle ReloadingHandle;
				float Delay = 1.2f + (i * 0.2f); // 1.2, 1.4, 1.6, 1.8, 2.0

				GetWorldTimerManager().SetTimer(ReloadingHandle, [this]()
				{
					UGameplayStatics::PlaySoundAtLocation(this, ReloadingSound,
					                                      GetMesh()->GetSocketLocation(TEXT("Muzzle_Socket_01")));
					UE_LOG(LogTemp, Warning, TEXT("중간 사운드 재생!"));
				}, Delay, false);
				ReloadTimerHandles.Add(ReloadingHandle);
			}

			GetWorldTimerManager().SetTimer(ReloadEndTimer, [this]()
			{
				UGameplayStatics::PlaySoundAtLocation(this, ReloadOpenMiddleSound,
				                                      GetMesh()->GetSocketLocation(TEXT("Muzzle_Socket_01")));
				UE_LOG(LogTemp, Warning, TEXT("3초 사운드 재생 성공"));
			}, 3.0f, false);
			ReloadTimerHandles.Add(ReloadEndTimer);

			GetWorldTimerManager().SetTimer(ReloadEndTimer, [this]()
			{
				UGameplayStatics::PlaySoundAtLocation(this, ReloadOpenMiddleSound,
				                                      GetMesh()->GetSocketLocation(TEXT("Muzzle_Socket_01")));
				UE_LOG(LogTemp, Warning, TEXT("4.0초 사운드 재생 성공"));
			}, 4.0f, false);
			ReloadTimerHandles.Add(ReloadEndTimer);
		}
	}

	bIsReloading = true;
	bCanFire = false;

	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("재장전중 %d/%d"), CurrentClip, MaxClip));

	float ReloadTime = 4.f;
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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("장전완료 %d/%d"), CurrentClip, MaxClip));
}

void AABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (KillCount ==0)
	{
		if(TargetDoor1&& !bIsDoorOpen1)
		{
			FRotator NewRotation = TargetDoor1->GetActorRotation();
			NewRotation.Yaw += 90.0f;
			TargetDoor1->SetActorRotation(NewRotation);
			bIsDoorOpen1 = true;
		}
	}
	
	if (KillCount >=30)
	{
		if(TargetDoor2&& !bIsDoorOpen2)
		{
			FRotator NewRotation = TargetDoor2->GetActorRotation();
			NewRotation.Yaw += 90.0f;
			TargetDoor2->SetActorRotation(NewRotation);
			bIsDoorOpen2 = true;
		}
	}
	
	if (KillCount >=60)
	{
		if(TargetDoor3&& !bIsDoorOpen3)
		{
			FRotator NewRotation = TargetDoor3->GetActorRotation();
			NewRotation.Yaw += 90.0f;
			TargetDoor3->SetActorRotation(NewRotation);
			bIsDoorOpen3 = true;
		}
	}
	
	if (KillCount >= 100)
	{
		if(TargetDoor4&& !bIsDoorOpen4)
		{
			FRotator NewRotation = TargetDoor4->GetActorRotation();
			NewRotation.Yaw += 90.0f;
			TargetDoor4->SetActorRotation(NewRotation);
			bIsDoorOpen4 = true;
		}
		
		FVector CurrentLocation = GetActorLocation();
		FVector TargetLocation = FVector(-2775.0f, 6137.0f, 193.0f);
		float Distance = FVector::Dist(CurrentLocation, TargetLocation);
		if (Distance <= 1000.f)
		{
			SaveStateToGI();
			UGameplayStatics::OpenLevel(GetWorld(), FName("TestLevel22"));
		}
	}
	if (!bIsSprint && !bIsStealthMode)
	{
		Stamina = FMath::Min(Stamina + (10.0f * DeltaTime), MaxStamina);
	}
	if (bIsStealthMode)
	{
		Stamina -= 40 * DeltaTime;
		if (Stamina < 0.0f)
		{
			Stamina = 0.0f;
			Stealth(true);
		}
	}
	if (!FMath::IsNearlyZero(RecoilRemainingPitch, 0.01f))
	{
		float PitchToApply = FMath::FInterpTo(0.0f, RecoilRemainingPitch, DeltaTime, 15.0f);
		AddControllerPitchInput(PitchToApply);


		TotalRecoilToRecover -= PitchToApply;

		RecoilRemainingPitch -= PitchToApply;
	}

	else if (!FMath::IsNearlyZero(TotalRecoilToRecover, 0.01f))
	{
		float RecoveryAmount = FMath::FInterpTo(0.0f, TotalRecoilToRecover, DeltaTime, 5.0f);

		AddControllerPitchInput(RecoveryAmount);
		TotalRecoilToRecover -= RecoveryAmount;
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
		if (StealthOnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, StealthOnSound, GetActorLocation());
		}
		for (UMaterialInstanceDynamic* Mat : CharacterMaterials)
		{
			if (Mat)
			{
				Mat->SetScalarParameterValue(TEXT("Opacity"), 0.11f);
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
				Mat->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
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
			Mat->SetScalarParameterValue(TEXT("opacity"), NewOpacity);
		}
	}
}

void AABaseCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	if (DieMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DieMontage);
		}

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			DisableInput(PC);
		}


		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
		}

		DetachFromControllerPendingDestroy();


		FTimerHandle FreezeTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (GetMesh())
			{
				GetMesh()->SetAnimInstanceClass(nullptr);
				GetMesh()->SetComponentTickEnabled(false);
			}
		}), 1.8f, false);

		FTimerHandle RestartTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RestartTimerHandle, this, &AABaseCharacter::RestartLevel, 3.5f, false);
	}
}

void AABaseCharacter::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName("GameStart"));
}

void AABaseCharacter::AddKill()
{
	KillCount++;
	Healing++;
	GrenadeAddCount++;
	// 20마리잡을때마다 20회복
	if (Healing == 20)
	{
		if (CurrentHP < 100)
		{
			CurrentHP += 20;
		}
		Healing = 0;
	}
	//40마리 잡을때마다 수류탄 1개지급
	if (GrenadeAddCount == 40)
	{
		GrenadeCount++;
		GrenadeAddCount = 0;
	}
	if (KillCount == 50)
	{
		MaxClip = 30;
	}
	// 최대 스테미나 증가
	if (KillCount == 60)
	{
		MaxStamina = 150;
	}
	// 대용량 탄창으로교체
	if (KillCount == 100)
	{
		MaxClip = 40;
		MaxStamina = 150;
	}
	
}

void AABaseCharacter::SaveStateToGI()
{
	UExodusGameInstance* GI = Cast<UExodusGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SaveCurrentClip = this->CurrentClip;
		GI->SaveCurrentGrenade = this->GrenadeCount;
		GI->SaveCurrentHp = this->CurrentHP;
		GI->SaveKillCount = this->KillCount;
		GI->SaveGrenadeAddCount = this->GrenadeAddCount;
		GI->SaveCurrentReserveAmmo = this->CurrentReserveAmmo;
	}
}
