#include "BossMonster.h"

bool ABossMonster::PerformAttack(AActor* Target)
{
	bool bResult = Super::PerformAttack(Target);

	return bResult;
}