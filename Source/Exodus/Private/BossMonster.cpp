// Copyright © 2026 비전공회담. All rights reserved.


#include "BossMonster.h"

void ABossMonster::BeginPlay()
{
	Super::BeginPlay();

	MaxHP = 500.0f;
	AttackDamage = 15.0f;
	CurrentHP = MaxHP;
	}

void ABossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePhase();
}

void ABossMonster::UpdatePhase()
{
	float HPPercent = CurrentHP / MaxHP;

	if (HPPercent < 0.6f)
		CurrentPhase = EBossPhase::Phase2;

	if (HPPercent < 0.3f)
		CurrentPhase = EBossPhase::Phase3;
}

void ABossMonster::PerformAttack(AActor* Target)
{
	if (!CanAttack()) return;

	switch (CurrentPhase)
	{
	case EBossPhase::Phase1:
		AttackDamage = 15.f;
		break;

	case EBossPhase::Phase2:
		AttackDamage = 25.f;
		break;

	case EBossPhase::Phase3:
		AttackDamage = 40.f;
		break;
	}
	Super::PerformAttack(Target);
}