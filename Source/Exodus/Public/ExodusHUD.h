// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExodusHUD.generated.h"

class UTextBlock;

UCLASS()
class EXODUS_API UExodusHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Timer;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int32 StartTimeSeconds = 600;
		
	int32 TotalSeconds;

	void UpdateTimer();
};
