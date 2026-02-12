// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "BossMonster.generated.h"


UENUM()
enum class EBossPhase : uint8
{
	Phase1,
	Phase2,
	Phase3
};

UCLASS()
class EXODUS_API ABossMonster : public AMonsterBase
{
	GENERATED_BODY()
	
public:
	EBossPhase CurrentPhase = EBossPhase::Phase1;

	virtual void Tick(float DeltaTime) override;
	virtual void PerformAttack(AActor* Target) override;

	void UpdatePhase();

protected:
	virtual void BeginPlay() override;

	
};
