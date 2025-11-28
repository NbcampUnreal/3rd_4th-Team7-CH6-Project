#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHCombinationLock.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class UUserWidget;

USTRUCT(BlueprintType)
struct FDigit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 CurrentDigit = 0;
};

UCLASS()
class SOH_API ASOHCombinationLock : public ASOHInteractableActor
{
	GENERATED_BODY()
	
public:
    ASOHCombinationLock();
    virtual void Interact_Implementation(AActor* Caller) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Body;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Head;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Gear1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Gear2;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Gear3;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* CameraComponent;

    // Gears array
    UPROPERTY()
    TArray<UStaticMeshComponent*> Gears;
    int32 CurrentGearIndex = 0;
	UPROPERTY()
	TArray<int32> GearValues;
	UPROPERTY(EditAnywhere)
	TArray<FDigit> CorrectCombination;

	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<UUserWidget> LockGuideWidgetClass;
    UPROPERTY()
    UUserWidget* LockGuideWidget;
	
	void Up();
	void Down();
	void Left();
	void Right();
	void Exit();
	void CheckCombination();
};
