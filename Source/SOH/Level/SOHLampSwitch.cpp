#include "SOHLampSwitch.h"
#include "SOHLamp.h"
#include "Components/StaticMeshComponent.h"

ASOHLampSwitch::ASOHLampSwitch()
{
    PrimaryActorTick.bCanEverTick = false;

    SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
    SwitchMesh->SetupAttachment(RootComponent);
}

void ASOHLampSwitch::BeginPlay()
{
    Super::BeginPlay();
}

void ASOHLampSwitch::Interact_Implementation(AActor* Caller)
{
    Super::Interact_Implementation(Caller);

    ToggleAllLamps();
}

bool ASOHLampSwitch::CanReceiveTrace_Implementation(AActor* Caller, bool bCanInteract)
{
    const bool bResult = Super::CanReceiveTrace_Implementation(Caller, bCanInteract);

    return bResult;
}

void ASOHLampSwitch::ToggleAllLamps()
{
    for (ASOHLamp* Lamp : ControlledLamps)
    {
        if (IsValid(Lamp))
        {
            Lamp->ToggleLight();
        }
    }
}