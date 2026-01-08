#pragma once

#include "CoreMinimal.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHLampSwitch.generated.h"

class UStaticMeshComponent;
class ASOHLamp;
class USoundBase;

UCLASS()
class SOH_API ASOHLampSwitch 
    : public ASOHInteractableActor
    , public ISOHSaveObjectInterface
{
    GENERATED_BODY()

public:
    ASOHLampSwitch();

protected:
    virtual void BeginPlay() override;

    virtual void Interact_Implementation(AActor* Caller) override;

    virtual bool CanReceiveTrace_Implementation(AActor* Caller, bool bCanInteract) override;

    void ApplyPowerState();
    
    void ToggleAllLamps();

    UFUNCTION(BlueprintImplementableEvent, Category = "Lamp")
    void PlaySwitchAnimation();

protected:
    UPROPERTY(EditAnywhere, Category="Lamp")
    bool bPowerOn = false;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* SwitchMesh;

public:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Lamp")
    TArray<ASOHLamp*> ControlledLamps;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lamp|Sound")
    USoundBase* ToggleSound;

    // 배전 연결을 위한 함수와 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Lock")
    bool bIsLocked = false;

    UFUNCTION(BlueprintCallable, Category = "Lamp|Lock")
    void SetLocked(bool bNewLocked);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FName WorldStateID;
    
    virtual void SaveState_Implementation(USOHSaveGame* SaveData) override;
    virtual void LoadState_Implementation(USOHSaveGame* SaveData) override;
};