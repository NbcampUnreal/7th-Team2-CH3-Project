#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterBase.h" 
#include "MonsterAIController.generated.h"

UCLASS()
class EXODUS_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY()
	AMonsterBase* ControlledMonster = nullptr;

	UPROPERTY()
	APawn* PlayerPawn = nullptr;

	void UpdateAI();
};
