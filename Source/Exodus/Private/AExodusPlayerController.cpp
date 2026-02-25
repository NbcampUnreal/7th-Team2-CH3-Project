// Copyright © 2026 비전공회담. All rights reserved.


#include "AExodusPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

AAExodusPlayerController::AAExodusPlayerController() :
	InputMappingContext(nullptr),
	MoveAction(nullptr)
	, JumpAction(nullptr)
	, LookAction(nullptr)
	, SprintAction(nullptr)
	, FireAction(nullptr)
	, ReloadAction(nullptr)
	,ThrowGrenadeAction(nullptr)
	,StealthAction(nullptr)
	,GetItemAction(nullptr)
{

}
void AAExodusPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	if (HUDWidgetClass)
	{
		UUserWidget* HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}