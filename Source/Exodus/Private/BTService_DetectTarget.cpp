// Copyright © 2026 비전공회담. All rights reserved.

#include "BTService_DetectTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterBase.h"

UBTService_DetectTarget::UBTService_DetectTarget()
{
    NodeName = "Detect Target";
    Interval = 0.5f;
    RandomDeviation = 0.1f;
}

void UBTService_DetectTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController || !AIController->GetPawn()) return;

    AMonsterBase* Monster = Cast<AMonsterBase>(AIController->GetPawn());
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (!PlayerCharacter)
    {
        OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);
        return;
    }

    float Distance = FVector::Dist(Monster->GetActorLocation(), PlayerCharacter->GetActorLocation());

    if (Distance <= DetectRadius && PlayerCharacter->ActorHasTag(TEXT("Player")))
    {
        if (Monster && !Monster->GetDetected())
        {
            Monster->SetDetected(true);
            Monster->PlayRoar(); 
        }

        if (Monster && !Monster->IsRoaring())
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsObject(
                TargetKey.SelectedKeyName,
                PlayerCharacter
            );
        }
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);

        if (Monster)
        {
            Monster->SetDetected(false);
        }
    }
}