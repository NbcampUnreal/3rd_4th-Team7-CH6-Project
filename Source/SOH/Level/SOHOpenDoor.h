#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHOpenDoor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USoundBase;
class ASOHLockActor;

UCLASS()
class SOH_API ASOHOpenDoor : public ASOHInteractableActor
{
	GENERATED_BODY()

public:
	ASOHOpenDoor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|State")
	bool bIsOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|State")
	bool bIsMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|State")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Sound")
	USoundBase* OpenSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Sound")
	USoundBase* CloseSound = nullptr;

	virtual void Interact_Implementation(AActor* Caller) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OpenDoor(AActor* Caller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_CloseDoor(AActor* Caller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void BP_OnLocked(AActor* Caller); // 문 열 수 없다는 로직을 만들기 위해 남겨둠

public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	void NotifyDoorMoveStarted();

	UFUNCTION(BlueprintCallable, Category = "Door")
	virtual void NotifyDoorMoveFinished(bool bNowOpen);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock")
	ASOHLockActor* LinkedLockActor;

	UFUNCTION(BlueprintCallable, Category = "Door|Lock")
	void UnlockOpenDoor(AActor* Caller);

	// Noise Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Noise")
	bool bUseNoiseEvent = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Noise")
	float NoiseLoudness = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Noise")
	float NoiseMaxRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Noise")
	FName NoiseTag = "Door";
};