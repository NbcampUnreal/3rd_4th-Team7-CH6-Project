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
	float WalkSpeed = 300.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed = 600.f;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsRunning = false;

	//입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Run;

	//이동 입력
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);

	//카메라 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
