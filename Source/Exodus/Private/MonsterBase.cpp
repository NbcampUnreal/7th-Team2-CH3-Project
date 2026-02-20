#include "MonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
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

float AMonsterBase::GetHp() const
{
	return CurrentHP;
}

void AMonsterBase::ReceiveDamage(float DamageAmount)
{
	if (bIsDead) return;

	CurrentHP -= DamageAmount;

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

	// 몽타주 재생
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}

	// 실제 데미지 적용
	float ActualDamage = UGameplayStatics::ApplyDamage(
		Target,
		AttackDamage,
		GetController(),
		this,
		nullptr
	);

	// 쿨타임 시작
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


	// 이동 정지
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	// 충돌 제거
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// AI 분리
	//DetachFromControllerPendingDestroy();

	// 3초 후 제거 (죽는 모션 보여주기용)
	GetWorldTimerManager().SetTimer(
		DeathTimer,
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