#include "ExodusHUD.h"
#include "TimerManager.h"
#include "ABaseCharacter.h" 
#include "ExodusGameInstance.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/TextBlock.h"

void UExodusHUD::NativeConstruct()
{
    Super::NativeConstruct();

    TotalSeconds = StartTimeSeconds;

    UExodusGameInstance* GI = Cast<UExodusGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    if (GI && GI->SaveRemainingTime > 0)
    {
        TotalSeconds = GI->SaveRemainingTime;
    }
    else
    {
        TotalSeconds = StartTimeSeconds;    
    }

    if (Txt_Timer)
    {
        int32 Minutes = TotalSeconds / 60;
        int32 Seconds = TotalSeconds % 60;
        Txt_Timer->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
    }

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UExodusHUD::UpdateTimer, 1.0f, true);
}

void UExodusHUD::UpdateTimer()
{
    TotalSeconds--;

    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;

    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    if (Txt_Timer)
    {
        Txt_Timer->SetText(FText::FromString(TimeString));
    }

    if (TotalSeconds <= 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

        ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

        if (AABaseCharacter* Hunter = Cast<AABaseCharacter>(PlayerCharacter))
        {
            Hunter->Die();
        }
    }

    UExodusGameInstance* GI = Cast<UExodusGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GI)
    {
        GI->SaveRemainingTime = TotalSeconds;
    }
}