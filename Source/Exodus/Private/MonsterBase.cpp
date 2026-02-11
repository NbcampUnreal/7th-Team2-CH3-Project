// Copyright © 2026 비전공회담. All rights reserved.


#include "MonsterBase.h"
#include "Kismet/GameplayStatics.h"

AMonsterBase::AMonsterBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
	
}

void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AMonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}
	
	float FinalDamage = FMath::Max(DamageAmount - Defense, 1.0f);
	CurrentHP -= FinalDamage;

	if (CurrentHP < 0.0f)
	{
		Die();
	}
	return FinalDamage;
}

void AMonsterBase::Die()
{
	bIsDead = true;
	GetMesh()->SetSimulatePhysics(true);
	SetLifeSpan(5.0f);
}

bool AMonsterBase::CanAttack() const
{
	return GetWorld()->TimeSeconds - LastAttackTime >= AttackCooldown;
}

void AMonsterBase::PerformAttack(AActor* Target)
{
	if (!Target || !CanAttack()) return;

	UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, nullptr);
	LastAttackTime = GetWorld()->TimeSeconds;
}