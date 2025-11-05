#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SOHPlayerCharacter.generated.h"

UCLASS()
class SOH_API ASOHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	//이동 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RotationRate = 500.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 200.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed = 400.f;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsRunning = false;

	//카메라 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	// 타이머 기반 감지 함수
	void TraceForInteractable();

public:
	//이동 입력
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void ToggleCrouch();


private:
	// 이전에 바라봤던 아이템
	AActor* LastHighlightedItem = nullptr;

	// 타이머 핸들
	FTimerHandle TraceTimerHandle;
};
