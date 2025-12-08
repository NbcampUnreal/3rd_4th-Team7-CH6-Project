#pragma once

#include "CoreMinimal.h"
#include "Level/SOHOpenDoor.h"
#include "SOHCabinet.generated.h"

class ASOHJumpScareBase;
class UCameraComponent;
class USceneComponent;
class ASOHPlayerCharacter;


UCLASS()
class SOH_API ASOHCabinet : public ASOHOpenDoor
{
	GENERATED_BODY()
	
public:

	ASOHCabinet();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cabinet")
	USceneComponent* HidePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cabinet")
	UCameraComponent* HideCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cabinet|State")
	bool bIsHidden = false;

	bool bPendingEnter = false;

	FVector CachedPlayerLocation;
	FRotator CachedPlayerRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cabinet|JumpScare")
	bool bUseCabinetJumpScare = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cabinet|JumpScare",
		meta = (EditCondition = "bUseCabinetJumpScare"))
	ASOHJumpScareBase* CabinetJumpScare = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cabinet|JumpScare",
		meta = (EditCondition = "bUseCabinetJumpScare"))
	bool bJumpScareOnlyOnce = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cabinet|JumpScare")
	bool bJumpScareTriggered = false;

	virtual void Interact_Implementation(AActor* Caller) override;
	virtual void NotifyDoorMoveFinished(bool bNowOpen) override;
	void EnterCabinet(ASOHPlayerCharacter* Player);
	void ExitCabinet(ASOHPlayerCharacter* Player);
};
