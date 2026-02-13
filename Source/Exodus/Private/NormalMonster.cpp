// Copyright © 2026 비전공회담. All rights reserved.

#include "NormalMonster.h"
#include "MonsterAIController.h"

ANormalMonster::ANormalMonster()
{
	AIControllerClass = AMonsterAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}