// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "NormalMonster.generated.h"


UCLASS()
class EXODUS_API ANormalMonster : public AMonsterBase
{
	GENERATED_BODY()

protected:
	virtual void Die() override;
	virtual void BeginPlay() override;

	void DropItem();

public:
	UPROPERTY(EditAnywhere, Category = "DropItem")
	TSubclassOf<AActor> GrenadeItemClass;

	UPROPERTY(EditAnywhere, Category = "DropItem")
	TSubclassOf<AActor> HolyItemClass;	
};
