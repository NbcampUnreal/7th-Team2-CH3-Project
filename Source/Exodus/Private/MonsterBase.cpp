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

	float ActualDamage = UGameplayStatics::ApplyDamage(
		Target,
		AttackDamage,
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

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}
	
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	DetachFromControllerPendingDestroy();


	GetWorldTimerManager().SetTimer(
		DeathTimer,
		this,
		&AMonsterBase::DestroyAfterDeath,
		3.f,
		false

	);

	DropItem();
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