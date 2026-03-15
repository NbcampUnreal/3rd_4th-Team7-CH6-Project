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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight|LightFunction")
    UMaterialInterface* LightFunctionMaterial = nullptr;
    // 상태 조회 추가 : bOn 동기화용
    UFUNCTION(BlueprintPure, Category = "Flashlight")
    bool IsOn() const;

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
    UMaterialInstanceDynamic* LightFunctionDynamic; // 실제 손전등 빛이 맺히는 것 같은 효과를 주기 위해 추가

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

    // 한 번만 보여줄 저전력 경고 플래그
    bool bLowBatteryWarned = false;

    void SetOn(bool bEnable);
    void SetEquipped(ACharacter* NewOwner);

    void SetLightFunctionMaterial(UMaterialInterface* NewMaterial);

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

    UPROPERTY(EditAnywhere, Category = "Flashlight|Sound")
    USoundBase* BatteryChargeSound;

    bool IsBatteryEmpty() const { return CurrentBattery <= 0.f; }

    // 베터리 잔량에 따른 빛 크기 조절
    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MaxIntensity = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MinIntensity = 0.01f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MaxOuterConeAngle = 35.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MinOuterConeAngle = 20.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MaxInnerConeAngle = 20.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MinInnerConeAngle = 10.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MaxAttenuationRadius = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Flashlight|Light")
    float MinAttenuationRadius = 500.f;

    void UpdateLightFromBattery();

    void StartBatteryDrain();

    void StopBatteryDrain();

    void DrainOnce();

    FTimerHandle BatteryDrainTimer;
};