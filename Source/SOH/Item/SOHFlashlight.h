#pragma once

#include "CoreMinimal.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "Item/SOHBaseItem.h"
#include "SOHFlashlight.generated.h"

class USceneComponent;
class USpotLightComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class ACharacter;
class ASOHJumpScareBase;

UCLASS()
class SOH_API ASOHFlashlight  : public ASOHBaseItem
{
    GENERATED_BODY()

public:
    ASOHFlashlight();

    virtual void Tick(float DeltaTime) override;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight|Sound")
    USoundBase* FlashlightOnSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight|Sound")
    USoundBase* FlashlightOffSound;
    
    virtual void SaveState_Implementation(class USOHSaveGame* SaveData) override;
    virtual void LoadState_Implementation(class USOHSaveGame* SaveData) override;

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

    UPROPERTY()
    UCameraComponent* OwnerCamera;

    UPROPERTY(EditAnywhere, Category = "Flashlight")
    FName HandSocketName = TEXT("Chest_Socket");

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