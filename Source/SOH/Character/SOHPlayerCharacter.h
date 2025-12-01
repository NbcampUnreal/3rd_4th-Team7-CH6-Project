#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SOHPlayerCharacter.generated.h"

class ASOHFlashlight;
class UAnimMontage;
class USoundBase;
class UUserWidget;

UCLASS()
class SOH_API ASOHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHPlayerCharacter();

public:
	//GameInstance에서 호출하기 위해 만들었습니다.
	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	float GetStamina() const { return Stamina; }

	// Battery 호출용

	void SetFlashlight(ASOHFlashlight* InLight) { Flashlight = InLight; }

	ASOHFlashlight* GetFlashlight() const { return Flashlight; }

	// CrossHair 호출용 bool 변수

	UPROPERTY(BlueprintReadOnly)
	bool bUIHit;

protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RotationRate = 500.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 200.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed = 400.f;

	FVector2D CurrentMoveInput;

	// 여기서부터

	// 체력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Health")
	float Health;

	// 스태미너

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float StaminaDrainPerSec = 10.f;   // 달릴 때 초당 소모

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float StaminaRegenPerSec = 10.f;   // 쉬면 초당 회복

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float MinStaminaToRun = 5.f;       // 이 이하이면 달리기 불가


	// 체력 감소/죽음 처리

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	USoundBase* DeathSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	bool bIsDead = false;

	void Die();

	UFUNCTION()
	void CallGameModeOnPlayerDied();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	// 여기까지 구현.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinPitchAngle = -60.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxPitchAngle = 60.f;

	// Flashlight 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	bool bFlashlightOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FlashlightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	ASOHFlashlight* Flashlight;

	void TraceForInteractable();
	
	// 상호작용 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	float TraceDistance = 300.f;
	
public:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void ToggleCrouch();
	void Interact();
	void ToggleFlashlight();
	void PlayUpperBodyMontage(UAnimMontage* Montage);
	void UseBattery();

	// 체력 관련 UI

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateOverlay(float HealthValue, float MaxHealthValue);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UUserWidget* PlayerHUD = nullptr;

private:
	UPROPERTY()
	AActor* LastHighlightedItem = nullptr;

	FTimerHandle TraceTimerHandle;
	FTimerHandle CrouchMovementCheckTimer;
	void CheckCrouchMovement();
};
