#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

class AABaseCharacter;

UCLASS()
class EXODUS_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();

protected:
	virtual void BeginPlay() override;

	

	UPROPERTY(EditAnywhere,BluePrintReadWrite, Category = "Stats")
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHP;

	bool bIsDead = false;

public:

	UFUNCTION(BlueprintCallable)
	int32 GetHp() const;

	void SetHp(int32 NewHp);

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(int32 DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformAttack(AActor* Target);

	bool IsDead() const { return bIsDead; }

	

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 2.f;

	UPROPERTY(EditAnywhere, Category = "Item")
	TSubclassOf<AActor> DropItemClass;

	bool CanAttack(AActor* Target) const;

	virtual void Die();
	void DropItem();

protected:

	bool bCanAttack = true;

	FTimerHandle AttackCooldownTimer;

	void ResetAttack();

	FTimerHandle DeathTimer;

	void DestroyAfterDeath();
};
