
// Copyright © 2026 비전공회담. All rights reserved.

#pragma once
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Grenade.h"
#include "ABaseCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class AABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHP;
	
	AABaseCharacter();
	
	// 인벤토리 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AActor*> Inventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxInventorySlots = 10;
	
	UFUNCTION(BlueprintCallable)
	bool AddItemToInventory(AActor* NewItem);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class AActor* CurrentOverlappedItem;
	
	
	
	
	void SetHp(int32 NewHp) { CurrentHP = NewHp; }
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GrenadeMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FireMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DieMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitFrontMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitBackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitRightMontage;
	
	
	
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
	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 CurrentClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 CurrentReserveAmmo;

	

public:
	// 총알 이펙트담음 배열들
	TArray<FName> MuzzleSocketNames;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* TrailEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsDead;
	
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//위젯함수들
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WBP_CrossLine;
	
	int32 GetHp(){return CurrentHP;}
	void RealLaunch();
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
	void StartSprint(const FInputActionValue& Value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	UFUNCTION()
	void StartFire(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Fire(int32 SocketIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Stealth(bool bIsForce);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnItemActionPressed(const FInputActionValue& Value);
	
	//공격력
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> WeaponMaterials;
	
	void SetWeaponOpacity(float opacity);
	void SetWeaponOpacity1(float opacity);
	
	int32 Attack;
	//죽었나?
	bool bIsdie;
	UFUNCTION()
	void Die();
	UFUNCTION()
	// 데미지 처리
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsReloading;
	bool bIsStealthMode;
	
	
	//스테미나 
	FTimerHandle StaminaTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxStamina;
	
	void UpdateStamina();
	
	//수류탄
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Grenade")
	TSubclassOf<class AGrenade> GrenadeClass;
	void LaunchGrenade();
	FTimerHandle LaunchTimerHandle;
	float LaunchTimer;
	bool bCanLaunch;
	void bLaunch();
	int32 GrenadeCount;
	
	
	float GetCurrentHp(){return CurrentHP;}
	//은신 딜레이
	FTimerHandle StealthTimerHandle;
	void StealthCoolDown();
		
	bool bIsStealthCooldown = false;
	
	TArray<class UMaterialInstanceDynamic*> CharacterMaterials;
	//void UpdateHp();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UNiagaraSystem* HitEffectSystem;
	
	void PlayHitEffect(const FHitResult& Hitd);
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadOpenEndSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadingSound;
	
	TArray<FTimerHandle> ReloadTimerHandles;
};		