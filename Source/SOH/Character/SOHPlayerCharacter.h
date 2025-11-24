#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SOHPlayerCharacter.generated.h"

class ASOHFlashlight;
class UAnimMontage;
class USoundBase;

UCLASS()
class SOH_API ASOHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override; // 틱함수 추가

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RotationRate = 500.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 200.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed = 400.f;

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

	// Flashlight 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	bool bFlashlightOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FlashlightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	ASOHFlashlight* Flashlight;

	void TraceForInteractable();

public:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void ToggleCrouch();
	void Interact();
	void ToggleFlashlight();
	void PlayUpperBodyMontage(UAnimMontage* Montage);


private:
	UPROPERTY()
	AActor* LastHighlightedItem = nullptr;

	FTimerHandle TraceTimerHandle;
};
