#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHSlidingDoor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USoundBase;

UCLASS()
class SOH_API ASOHSlidingDoor : public ASOHInteractableActor
{
	GENERATED_BODY()
	
public:
	ASOHSlidingDoor();

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
	void BP_OnLocked(AActor* Caller);

public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	void NotifyDoorMoveStarted();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void NotifyDoorMoveFinished(bool bNowOpen);

};