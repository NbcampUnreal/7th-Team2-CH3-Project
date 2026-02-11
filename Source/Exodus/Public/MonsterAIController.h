// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"


UCLASS()
class EXODUS_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	virtual void Tick(float Deltatime) override;
	virtual void OnPossess(APawn* InPawn) override;

protected:
	
	UPROPERTY()
	class AMonsterBase* ControlledMonster;

	UPROPERTY()
	APawn* PlayerPawn;

	void UpdateAI();
};
