// Copyright © 2026 비전공회담. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
// KeySelector를 사용하기 위해 필요한 헤더입니다.
#include "BehaviorTree/BehaviorTreeTypes.h" 
#include "BTService_DetectTarget.generated.h"

UCLASS()
class EXODUS_API UBTService_DetectTarget : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_DetectTarget();

protected:
    // 서비스의 주기적 실행을 담당하는 핵심 함수
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 에디터에서 조절할 탐지 범위
    UPROPERTY(EditAnywhere, Category = "AI")
    float DetectRadius = 1000.f;

    // 어떤 블랙보드 키(Target)에 저장할지 선택하는 변수
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;
};