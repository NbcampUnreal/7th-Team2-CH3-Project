// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

UCLASS()
class EXODUS_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();

protected:
	virtual void BeginPlay() override;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHP = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Defense = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SightRange = 1200.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 2.0f;

	bool bIsDead = false;

	float LastAttackTime = 0.0f;

public:	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 600.f;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	virtual bool CanAttack() const;
	virtual void PerformAttack(AActor* Target);

	bool IsDead() const { return bIsDead; }
};
