
// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABaseCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class AABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movemevt")
	float NomalSpeed;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movemevt")
	float SprintSpeed;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movemevt")
	float SprintSpeedMultiplier;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void StartJump(const FInputActionValue& Value);

	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void StartSpirnt(const FInputActionValue& Value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);



};