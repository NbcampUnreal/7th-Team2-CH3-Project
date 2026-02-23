// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class EXODUS_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMonsterSpawner();

protected:
    virtual void BeginPlay() override;

    void SpawnMonster();

    UFUNCTION()
    void OnMonsterDestroyed(AActor* DestroyedActor);

public:
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<class AMonsterBase> MonsterClass;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnInterval = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnRadius = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Spawner")
    int32 MaxMonsterCount = 10;

private:
    FTimerHandle SpawnTimerHandle;

    UPROPERTY(VisibleAnywhere, Category = "Spawner|Debug")
    int32 CurrentMonsterCount = 0;
};