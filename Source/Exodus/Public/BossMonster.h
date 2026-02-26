#pragma once
#include "MonsterBase.h"
#include "BossMonster.generated.h"

UCLASS()
class EXODUS_API ABossMonster : public AMonsterBase
{
	GENERATED_BODY()

public:
	virtual bool PerformAttack(AActor* Target) override;
};