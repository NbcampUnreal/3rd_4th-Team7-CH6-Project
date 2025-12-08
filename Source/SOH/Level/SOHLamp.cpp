#include "SOHLamp.h"
#include "Components/StaticMeshComponent.h"
#include "Components/RectLightComponent.h"

ASOHLamp::ASOHLamp()
{
    PrimaryActorTick.bCanEverTick = false;

    LampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LampMesh"));
    RootComponent = LampMesh;

    RectLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLight"));
    RectLight->SetupAttachment(RootComponent);

    bIsOn = true;
}

void ASOHLamp::BeginPlay()
{
    Super::BeginPlay();

    if (RectLight)
    {
        bIsOn = RectLight->IsVisible();
    }
}

void ASOHLamp::ToggleLight()
{
    if (bIsOn)
    {
        TurnOff();
    }
    else
    {
        TurnOn();
    }
}

void ASOHLamp::TurnOn()
{
    bIsOn = true;

    if (LampMaterialInstance)
    {
        LampMaterialInstance->SetScalarParameterValue(TEXT("Emissive_Control"), 0.09f);
    }

    if (RectLight)
    {
        RectLight->SetVisibility(true);
    }
}

void ASOHLamp::TurnOff()
{
    bIsOn = false;

    if (LampMaterialInstance)
    {
        LampMaterialInstance->SetScalarParameterValue(TEXT("Emissive_Control"), 0.0f);
    }

    if (RectLight)
    {
        RectLight->SetVisibility(false);
    }
}
