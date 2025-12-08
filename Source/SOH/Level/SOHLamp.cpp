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

    bIsOn = false;
}

void ASOHLamp::BeginPlay()
{
    Super::BeginPlay();

    bIsOn = bStartOn;    
    UpdateLightState();
}

void ASOHLamp::ToggleLight()
{
    bIsOn = !bIsOn;
    UpdateLightState();
}

void ASOHLamp::TurnOn()
{
    bIsOn = true;
    UpdateLightState();
}

void ASOHLamp::TurnOff()
{
    bIsOn = false;

    UpdateLightState();
}

void ASOHLamp::UpdateLightState()
{
    // Emissive 제어
    if (LampMaterialInstance)
    {
        const float EmissiveValue = bIsOn ? EmissiveOn : EmissiveOff;
        LampMaterialInstance->SetScalarParameterValue(TEXT("Emissive_Control"), EmissiveValue);
    }

    // RectLight 제어
    if (RectLight)
    {
        if (bIsOn)
        {
            RectLight->SetVisibility(true);
            RectLight->SetIntensity(LightIntensity);
        }
        else
        {
            RectLight->SetVisibility(false);
            RectLight->SetIntensity(0.0f);
        }
    }
}
