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
    RandomDeviation = 0.f;  
}

void UBTService_DetectTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return;

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter) return;

    float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance <= DetectRadius && PlayerCharacter->ActorHasTag(TEXT("Player"))) 
    { 
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, PlayerCharacter); 
    }
    else
    {   
        OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName); 
    }
}