// Copyright © 2026 비전공회담. All rights reserved.

#include "MonsterSpawner.h"
#include "MonsterBase.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentMonsterCount = 0;
	MaxMonsterCount = 10;	
	SpawnInterval = 5.0f;
SpawnRadius = 500.0f;
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterSpawner::SpawnMonster, SpawnInterval, true);
}

void AMonsterSpawner::SpawnMonster()
{
if (CurrentMonsterCount >= MaxMonsterCount)
{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	if (!MonsterClass) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	FNavLocation RandomLocation;
	if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, RandomLocation))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AMonsterBase* NewMonster = GetWorld()->SpawnActor<AMonsterBase>(MonsterClass, RandomLocation.Location, FRotator::ZeroRotator, SpawnParams);

		if (NewMonster)
	{
			CurrentMonsterCount++;

			if (CurrentMonsterCount >= MaxMonsterCount)
		{
			GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
			}
		if (GEngine)
			{GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,FString::Printf(TEXT("Monster Spawned! (Current: %d / Max: %d)"), CurrentMonsterCount, MaxMonsterCount));
}
		}
	}
}

void AMonsterSpawner::OnMonsterDestroyed(AActor* DestroyedActor)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange,
			TEXT("Monster Destroyed."));
	}
}
