// Copyright © 2026 비전공회담. All rights reserved.

#include "BTTask_FindRandomLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSys)
		return EBTNodeResult::Failed;

	FVector Origin = ControlledPawn->GetActorLocation();
	FNavLocation RandomLocation;

	bool bSuccess = NavSys->GetRandomReachablePointInRadius(
		Origin,
		SearchRadius,
		RandomLocation
	);

	if (!bSuccess)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp =
		OwnerComp.GetBlackboardComponent();

	if (!BlackboardComp)
		return EBTNodeResult::Failed;

	BlackboardComp->SetValueAsVector(
		PatrolLocationKey.SelectedKeyName,
		RandomLocation.Location
	);

	return EBTNodeResult::Succeeded;
}