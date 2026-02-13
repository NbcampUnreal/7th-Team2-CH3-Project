
// Copyright © 2026 비전공회담. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "ABaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class AABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AABaseCharacter();

protected:
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 CurrentClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 CurrentReserveAmmo;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//위젯함수들
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WBP_CrossLine;
	
	UPROPERTY()
	UUserWidget* CrossLine;
//액션함수들
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

	UFUNCTION()
	void StartFire(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Fire();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Reload();

	//공격력
	int32 Attack;

	// 데미지 처리
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//라인트레이서 변수들
	FHitResult Hit;
	FVector ViewLocation;
	FRotator ViewRotation;
	FCollisionQueryParams Params;

	//총알딜레이
	FTimerHandle FireTimerHandle;
	bool bCanFire;
	void ResultFire();

	//장전딜레이
	FTimerHandle ReloadTimerHandle;
	void CompleteReload();
	
	//상태
	bool bIsSprint;
	
	//스테미나 
	FTimerHandle StaminaTimerHandle;
	float Stamina;
	float MaxStamina;
	void UpdateStamina();
	//void UpdateHp();
};