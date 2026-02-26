// Copyright © 2026 비전공회담. All rights reserved.

#include "BTTask_Attack.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (!Owner) return EBTNodeResult::Failed;

	AMonsterBase* Monster = Cast<AMonsterBase>(Owner->GetPawn());
	if (!Monster) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName)
	);

	if (!Target) return EBTNodeResult::Failed;

	bool bSuccess = Monster->PerformAttack(Target);

	return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}