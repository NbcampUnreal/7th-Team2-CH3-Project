#include "MonsterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "ABaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"

AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add(FName("Monster"));
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
}

int32 AMonsterBase::GetHp() const
{
	return CurrentHP;
}

void AMonsterBase::ReceiveDamage(int32 DamageAmount)
{
	if (bIsDead) return;

	CurrentHP -= DamageAmount;

	if (CurrentHP >= 0.f) 
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		FRotator SpawnRotation = GetActorRotation();

		if (BloodNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodNiagara, SpawnLocation, SpawnRotation);
		}

		if (FleshNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FleshNiagara, SpawnLocation, SpawnRotation);
		}
	}

	if (HitMontage && CurrentHP > 0.f)
	{
		PlayAnimMontage(HitMontage);
	}

	if (CurrentHP <= 0.f)
	{
		CurrentHP = 0.f;
		Die();
	}
}

bool AMonsterBase::CanAttack(AActor* Target) const
{
	if (!Target || bIsDead || !bCanAttack)
		return false;

	if (!Target->ActorHasTag(TEXT("Player")))
		return false;

	float Distance = FVector::Dist(
		GetActorLocation(),
		Target->GetActorLocation()
	);

	return Distance <= AttackRange;
}

bool AMonsterBase::PerformAttack(AActor* Target)
{
	if (!CanAttack(Target))
		return false;

	bCanAttack = false;

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
	float ActualDamage = 0;
	FHitResult DummyHit;
	DummyHit.ImpactPoint = Target->GetActorLocation();
	DummyHit.ImpactNormal = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();

	ActualDamage = UGameplayStatics::ApplyPointDamage(
		Target,
		20,
		GetActorForwardVector(),
		DummyHit,
		GetController(),
		this,
		nullptr
	);

	GetWorldTimerManager().SetTimer(
		AttackCooldownTimer,
		this,
		&AMonsterBase::ResetAttack,
		AttackCooldown,
		false
	);

	return ActualDamage > 0.f;
}

void AMonsterBase::ResetAttack()
{
	bCanAttack = true;
}

void AMonsterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	if (GetController())
	{
		GetController()->UnPossess();
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (DeathMontage)
	{
		float Duration = PlayAnimMontage(DeathMontage);

		if (Duration > 0.f)
		{
			GetWorldTimerManager().SetTimer(
				DeathTimer,
				[this]()
				{
					if (GetMesh())
					{
						GetMesh()->bNoSkeletonUpdate = true;
					}
				},
				Duration - 0.1f,
				false
			);
		}
	}

	DropItem();

	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AMonsterBase::DestroyAfterDeath,
		3.f,
		false
	);
}
void AMonsterBase::DestroyAfterDeath()
{
	Destroy();
}

void AMonsterBase::SetHp(int32 NewHp)
{
	CurrentHP = NewHp;
}

void AMonsterBase::DropItem()
{
	if (!DropItemClass) return;

	float RandomValue = FMath::FRandRange(0.0f, 100.0f);

	if (RandomValue <= 5.0f)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
		FRotator SpawnRotation = FRotator::ZeroRotator;

		GetWorld()->SpawnActor<AActor>(DropItemClass, SpawnLocation, SpawnRotation, SpawnParams);

		UE_LOG(LogTemp, Warning, TEXT("Item Dropped! (Prob: %.2f)"), RandomValue);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Item Dropped. (Prob: %.2f)"), RandomValue);
	}
}

void AMonsterBase::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);


	FVector Start = GetMesh()->GetSocketLocation(HandSocketName);
	FVector End = Start + (GetActorForwardVector() * 1.0f);

	float SphereRadius = 20.0f;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);

	// ECC_GameTraceChannel1은 플레이어가 'Block' 설정되어 있어야 합니다.
	bool bHasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		SphereShape,
		Params
	);

	// 4. 시각화 (디버그 구체)
	// 빨간색 구체는 실제 공격 판정의 크기와 위치를 보여줍니다.
	DrawDebugSphere(GetWorld(), Start, SphereRadius, 12, FColor::Red, false, 0.1f);

	// 5. 데미지 판정 로직
	// [중요] bHasHit이 true일 때만 내부 로직이 실행되도록 엄격히 제한합니다.
	if (bHasHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();

		// 플레이어 태그 확인 및 중복 타격 방지 리스트 확인
		if (HitActor->ActorHasTag(TEXT("Player")) && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);

			UGameplayStatics::ApplyDamage(
				HitActor,
				10.0f,
				GetController(),
				this,
				nullptr
			);

			// 타격 성공 시 로그와 시각적 피드백 (녹색 구체)
			UE_LOG(LogTemp, Warning, TEXT("!!! Player Hit Successfully !!!"));
			DrawDebugSphere(GetWorld(), HitResult.Location, SphereRadius + 5.0f, 12, FColor::Green, false, 0.5f);
		}
	}
}
