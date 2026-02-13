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


	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
	float CurrentHP;

	bool bIsDead = false;

public:

	UFUNCTION(BlueprintCallable)
	float GetHp() const;

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(float DamageAmount);

	bool IsDead() const { return bIsDead; }


	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	bool CanAttack(AActor* Target) const;


	bool PerformAttack(AActor* Target);
};