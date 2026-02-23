// Copyright © 2026 비전공회담. All rights reserved.

#include "BTService_DetectTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter)
    {
        OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);
        return;
    }

    float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), PlayerCharacter->GetActorLocation());

    // 거리 체크 + 태그 체크 (태그는 에디터의 Character 본체에 "Player"라고 적혀있어야 함)
    if (Distance <= DetectRadius && PlayerCharacter->ActorHasTag(TEXT("Player")))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, PlayerCharacter);
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);
    }
}