#include "MonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;
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

	if (CurrentHP <= 0.f)
	{
		CurrentHP = 0.f;
		bIsDead = true;
		Destroy();
	}
}

bool AMonsterBase::CanAttack(AActor* Target) const
{
	if (!Target || bIsDead)
		return false;

	float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	return Distance <= AttackRange;
}

bool AMonsterBase::PerformAttack(AActor* Target)
{
	if (!CanAttack(Target))
		return false;

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

	return ActualDamage > 0.f;
}