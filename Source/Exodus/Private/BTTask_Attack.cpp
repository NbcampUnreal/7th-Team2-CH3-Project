// Copyright © 2026 비전공회담. All rights reserved.
#include "BTTask_Attack.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
    bNotifyTick = true;
    // 중요: 여러 AI가 이 노드를 공유할 때 변수(ElapsedTime)가 섞이지 않도록 함
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* Owner = OwnerComp.GetAIOwner();
    if (!Owner) return EBTNodeResult::Failed;

    AMonsterBase* Monster = Cast<AMonsterBase>(Owner->GetPawn());
    if (!Monster) return EBTNodeResult::Failed;

    UObject* TargetObj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName);
    AActor* Target = Cast<AActor>(TargetObj);
    if (!Target) return EBTNodeResult::Failed;

    // 공격 방향으로 회전시키는 로직이 있으면 좋습니다.

    bool bActionStarted = Monster->PerformAttack(Target);

    if (bActionStarted)
    {
        if (Monster->AttackMontage)
        {
            // Monster->PerformAttack 내부에서 이미 PlayAnimMontage를 한다면 여기서 중복 호출 금지
            // 만약 PerformAttack이 로직만 처리한다면 아래 코드가 필요합니다.
            // Monster->PlayAnimMontage(Monster->AttackMontage); 

            AttackDuration = Monster->AttackMontage->GetPlayLength();
            ElapsedTime = 0.0f;
            return EBTNodeResult::InProgress;
        }
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ElapsedTime += DeltaSeconds;

    if (ElapsedTime >= AttackDuration)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}