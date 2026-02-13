// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AExodusPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;


UCLASS()
class AAExodusPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAExodusPlayerController();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Input") 
	UInputAction* FireAction;

	UPROPERTY(EditAnyWhere, BluePrintReadWrite, Category = "Input") 
	UInputAction* ReloadAction;

	virtual void BeginPlay() override;
};