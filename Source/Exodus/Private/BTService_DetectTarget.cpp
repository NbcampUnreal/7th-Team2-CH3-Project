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
    if (!AIController) return;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return;

    AMonsterBase* Monster = Cast<AMonsterBase>(Pawn);
    if (!Monster) return;

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

    if (!PlayerCharacter || !PlayerCharacter->ActorHasTag(TEXT("Player")))
    {
        BBComp->ClearValue(TargetKey.SelectedKeyName);
        Monster->SetDetected(false);
        return;
    }

    float Distance = FVector::Dist(Monster->GetActorLocation(), PlayerCharacter->GetActorLocation());

    if (Distance <= DetectRadius)
    {
        if (!Monster->GetDetected())
        {
            Monster->SetDetected(true);
            Monster->PlayRoar(); 
        }

        if (!Monster->IsRoaring())
        {
            BBComp->SetValueAsObject(TargetKey.SelectedKeyName, PlayerCharacter);
        }
    }
    else
    {
        BBComp->ClearValue(TargetKey.SelectedKeyName);
        Monster->SetDetected(false);
    }
}