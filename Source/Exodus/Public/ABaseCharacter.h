
// Copyright © 2026 비전공회담. All rights reserved.

#pragma once
#include "NiagaraFunctionLibrary.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Grenade.h"
#include "ABaseCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class ExodusGameInstance;
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 CurrentClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 CurrentReserveAmmo;

	FTimerHandle DeathTimerHandle;

	UFUNCTION()
	void RestartLevel();
	
	
	// 총알 이펙트담음 배열들
	TArray<FName> MuzzleSocketNames;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	 UParticleSystem* TrailEffect;
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
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
	USoundBase* ReloadOpenMiddleSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadingSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadingEndSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* StealthOnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill")
	int32 KillCount;
	int32 Healing;
	int32 GrenadeAddCount;
	
	UFUNCTION(BlueprintCallable, Category = "Kill")
	void AddKill();

	TArray<FTimerHandle> ReloadTimerHandles;
	
	float RecoilRemainingPitch = 0.0f;
	float RecoilRemainingYaw = 0.0f;
	float TotalRecoilToRecover = 0.0f;
	
	void SaveStateToGI();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	AActor* TargetDoor7;
	
	bool bIsDoorOpen1 = false;
	bool bIsDoorOpen2 = false;
	bool bIsDoorOpen3 = false;
	bool bIsDoorOpen4 = false;
	bool bIsDoorOpen5 = false;
	bool bIsDoorOpen6 = false;
	bool bIsDoorOpen7 = false;
	
	
	bool bIsDoor1Closed = false;
	bool bIsDoor2Closed = false;
	bool bIsDoor3Closed = false;
	bool bIsDoor4Closed = false;
	bool bIsDoor5Closed = false;
	bool bIsDoor6Closed = false;
	

	// 문주변 거리
	const float PassThreshold = 300.0f;
	
	FVector CloseDoorDirection1 = 	FVector(-300,-5895,35);
	FVector CloseDoorDirection2 = 	FVector(-5530,-5820,35);
	FVector CloseDoorDirection3 = 	FVector(-3115,-8525,35);
	FVector CloseDoorDirection4 = 	FVector(1450,-2295,-30);
	FVector CloseDoorDirection5 = 	FVector(-1270,-6650,0);
	

	UPROPERTY()
	TSubclassOf<UUserWidget> EndingClass;
	UPROPERTY()
	UUserWidget* EndingWidget;
	bool bIsEndingStarted = false;
	
	UFUNCTION()
	void OnLevelTransitionOverlap(UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult);
	
	bool bHasStageTransitioned =false;
};		