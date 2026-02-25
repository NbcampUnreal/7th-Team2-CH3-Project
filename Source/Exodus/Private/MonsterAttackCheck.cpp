 // Copyright © 2026 비전공회담. All rights reserved.


#include "MonsterAttackCheck.h"
#include "MonsterBase.h" 
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimNotifies/AnimNotifyState.h" 

void UMonsterAttackCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (MeshComp && MeshComp->GetOwner())
    {
        AMonsterBase* Monster = Cast<AMonsterBase>(MeshComp->GetOwner());
        if (Monster)
        {
            Monster->AttackCheck();
        }
    }
}

void UMonsterAttackCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (MeshComp && MeshComp->GetOwner())
    {
        AMonsterBase* Monster = Cast<AMonsterBase>(MeshComp->GetOwner());
        if (Monster)
        {
            Monster->ClearHitActors();
        }
    }
}