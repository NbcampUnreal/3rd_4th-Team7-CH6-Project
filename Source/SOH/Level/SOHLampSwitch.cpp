#include "SOHLampSwitch.h"
#include "SOHLamp.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASOHLampSwitch::ASOHLampSwitch()
{
    PrimaryActorTick.bCanEverTick = false;

    SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
    SwitchMesh->SetupAttachment(RootComponent);

    if (InteractionRange)
        InteractionRange->SetupAttachment(SwitchMesh);

    if (UIAnchor)
    {
        UIAnchor->SetupAttachment(SwitchMesh);
        UIAnchor->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
    }
}

void ASOHLampSwitch::BeginPlay()
{
    Super::BeginPlay();
}

void ASOHLampSwitch::Interact_Implementation(AActor* Caller)
{
    Super::Interact_Implementation(Caller);

    ToggleAllLamps();

    if (ToggleSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ToggleSound, GetActorLocation());
    }

    PlaySwitchAnimation();
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