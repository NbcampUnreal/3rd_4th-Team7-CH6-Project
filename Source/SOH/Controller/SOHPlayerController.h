#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SOH/Item/SOHFlashlight.h"
#include "SOHPlayerController.generated.h"

UCLASS()
class SOH_API ASOHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Run;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Crouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Interact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_ToggleFlashlight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_UseBattery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Heal;  

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* IA_TogglePause;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* IA_ToggleMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* IA_ToggleInventory;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartRun(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void ToggleCrouch(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void ToggleFlashlight(const FInputActionValue& Value);
	void UseBattery(const FInputActionValue& Value);
	void UseHealthItem(const FInputActionValue& Value);
	void OnTogglePause(const FInputActionValue& Value);
	void OnToggleMap(const FInputActionValue& Value);
	void OnToggleInventory(const FInputActionValue& Value);
};
