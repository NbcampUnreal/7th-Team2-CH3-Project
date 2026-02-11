// Copyright © 2026 비전공회담. All rights reserved.


#include "NormalMonster.h"


void ANormalMonster::BeginPlay()
{
	Super::BeginPlay();

	MaxHP = 100.f;
	AttackDamage = 10.f;
	CurrentHP = MaxHP;
}

void ANormalMonster::Die()
{
	Super::Die();
	DropItem();
}

void ANormalMonster::DropItem()
{
	float Roll = FMath::FRand();

	if (Roll <= 0.9f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drop Grenade"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Drop Holy"));
	}
}
