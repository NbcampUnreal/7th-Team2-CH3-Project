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

void AMonsterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (GetWorld())
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}
}

int32 AMonsterBase::GetHp() const
{
	return CurrentHP;
}

void AMonsterBase::ReceiveDamage(int32 DamageAmount)
{
	if (bIsDead) return;

	CurrentHP -= DamageAmount;

	UpdateHP_UI();

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

	ClearHitActors();

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}

	GetWorldTimerManager().SetTimer(
		AttackCooldownTimer,
		this,
		&AMonsterBase::ResetAttack,
		AttackCooldown,
		false
	);

	return true;
}

void AMonsterBase::ResetAttack()
{
	bCanAttack = true;
}

void AMonsterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (AABaseCharacter* Player = Cast<AABaseCharacter>(PlayerCharacter))
	{
		Player->AddKill();
	}

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

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetSimulatePhysics(false);
	}

	if (DeathMontage)
	{
		float Duration = PlayAnimMontage(DeathMontage);

		if (Duration > 0.f)
		{
			FTimerHandle DeathFreezeTimer;
			GetWorldTimerManager().SetTimer(
				DeathFreezeTimer,
				[WeakMonster = TWeakObjectPtr<AMonsterBase>(this)]()
				{
					if (AMonsterBase* StrongMonster = WeakMonster.Get())
					{
						if (StrongMonster->GetMesh())
						{
							StrongMonster->GetMesh()->bPauseAnims = true;
						}
					}
				},
				Duration,
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
	FVector End = Start + (GetActorForwardVector() * AttackRange);

	float SphereRadius = AttackRadius;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);

	bool bHasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		Params
	);

	if (bHasHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor->ActorHasTag(TEXT("Player")) && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			FVector ShotDirection = (End - Start).GetSafeNormal();
			UGameplayStatics::ApplyPointDamage(
				HitActor,
				AttackDamage,
				ShotDirection,
				HitResult,
				GetController(),
				this,
				nullptr
			);
		}
	}
}

void AMonsterBase::PlayRoar()
{
	if (bIsDead || bIsRoaring) return;

	bIsRoaring = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (RoarMontage)
	{
		float Duration = PlayAnimMontage(RoarMontage);

		if (Duration > 0.f)
		{
			FTimerHandle RoarTimer;
			GetWorldTimerManager().SetTimer(
				RoarTimer,
				[this]()
				{
					bIsRoaring = false;

					if (!bIsDead)
					{
						GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					}
				},
				Duration,
				false
			);
		}
	}
}
