#pragma once

#include "CoreMinimal.h"
#include "Item/SOHBaseItem.h"
#include "SOHFlashlight.generated.h"

class USceneComponent;
class USpotLightComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class ACharacter;
class ASOHJumpScareBase;

UCLASS()
class SOH_API ASOHFlashlight : public ASOHBaseItem
{
    GENERATED_BODY()

public:
    ASOHFlashlight();

    UFUNCTION(BlueprintCallable, Category = "Puzzle")
    void SetFlashlightCutScene();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Flashlight")
    void PlayFlashlightCutScene();

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void Toggle();

    UFUNCTION(BlueprintCallable, Category = "Flashlight|Battery")
    bool UseBatteryItem(float ChargeAmount);

    UFUNCTION(BlueprintPure, Category = "Flashlight|Battery")
    float GetBatteryPercent() const;

protected:
    virtual void BeginPlay() override;

    virtual void Interact_Implementation(AActor* Caller) override;

private:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Pivot;

    UPROPERTY(VisibleAnywhere)
    USpotLightComponent* Spot;

    UPROPERTY()
    ACharacter* OwnerChar;

    UPROPERTY()
    USkeletalMeshComponent* OwnerMesh;

    UPROPERTY(EditAnywhere, Category = "Flashlight")
    FName HandSocketName = TEXT("FlashlightSocket");

    UPROPERTY(EditAnywhere, Category = "Flashlight")
    bool bStartOn = false;

    bool bOn = false;
    bool bEquipped = false;

    void SetOn(bool bEnable);
    void SetEquipped(ACharacter* NewOwner);

    // 배터리

    UPROPERTY(EditAnywhere, Category = "Flashlight|Battery")
    float MaxBattery = 100.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Battery")
    float InitialBattery = 50.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Battery")
    float DrainRate = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Battery")
    float DrainInterval = 1.0f;

    UPROPERTY(VisibleInstanceOnly, Category = "Flashlight|Battery")
    float CurrentBattery = 3.0f;

    bool IsBatteryEmpty() const { return CurrentBattery <= 0.f; }

    void UpdateLightFromBattery();

    void StartBatteryDrain();

    void StopBatteryDrain();

    void DrainOnce();

    FTimerHandle BatteryDrainTimer;
};