#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ExodusGameInstance.generated.h"


UCLASS()
class EXODUS_API UExodusGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UExodusGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Settings")
	float Vol_Master;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Settings")
	float Vol_BGM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Settings")
	float Vol_Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Settings")
	float Vol_Zombie;
};