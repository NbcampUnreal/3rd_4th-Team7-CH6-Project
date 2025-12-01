#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHLampSwitch.generated.h"

class UStaticMeshComponent;
class ASOHLamp;

UCLASS()
class SOH_API ASOHLampSwitch : public ASOHInteractableActor
{
    GENERATED_BODY()

public:
    ASOHLampSwitch();

protected:
    virtual void BeginPlay() override;

    virtual void Interact_Implementation(AActor* Caller) override;

    virtual bool CanReceiveTrace_Implementation(AActor* Caller, bool bCanInteract) override;

    void ToggleAllLamps();

    UFUNCTION(BlueprintImplementableEvent, Category = "Lamp")
    void PlaySwitchAnimation();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* SwitchMesh;

public:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Lamp")
    TArray<ASOHLamp*> ControlledLamps;
};