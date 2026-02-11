// Copyright © 2026 비전공회담. All rights reserved.


#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"



void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledMonster = Cast<AMonsterBase>(InPawn);
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateAI();
}

void AMonsterAIController::UpdateAI()
{
    if (!ControlledMonster || ControlledMonster->IsDead() || !PlayerPawn) return;

    float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), ControlledMonster->GetActorLocation());

    
    if (Distance <= ControlledMonster->AttackRange)
    {
        StopMovement();
        ControlledMonster->PerformAttack(PlayerPawn);
        return;
    }

    
    MoveToActor(PlayerPawn, 100.f);
}
