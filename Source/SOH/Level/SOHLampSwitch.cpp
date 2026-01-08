#include "SOHLampSwitch.h"
#include "SOHLamp.h"
#include "GameMode/SOHSaveGame.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"

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

    bIsLocked = false;
}

void ASOHLampSwitch::BeginPlay()
{
    Super::BeginPlay();
}

void ASOHLampSwitch::Interact_Implementation(AActor* Caller)
{
    Super::Interact_Implementation(Caller);

    if (Caller)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LampSwitch] Caller Class: %s"),
            *Caller->GetClass()->GetName());
    }

    if (bIsLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LampSwitch] Switch is LOCKED - Showing message"));

        USOHMessageManager* MessageMgr = Caller->FindComponentByClass<USOHMessageManager>();

        if (MessageMgr)
        {
			FText Msg = FText::FromString(TEXT("불이 들어오지 않는다.. 전기실에 가보자."));

            MessageMgr->ShowMessageText(Msg, 2.0f);

            UE_LOG(LogTemp, Log, TEXT("[LampSwitch] Message displayed"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[LampSwitch] MessageManager NOT Found on Caller!"));
        }

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[LampSwitch] Switch is UNLOCKED - Toggling lights"));

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
    if (bIsLocked)
        return;

    // 🔥 스위치의 "논리 상태"를 먼저 토글
    bPowerOn = !bPowerOn;

    // 🔥 현재 상태를 램프들에 강제로 적용
    ApplyPowerState();
}

void ASOHLampSwitch::SetLocked(bool bNewLocked)
{
    bIsLocked = bNewLocked;
}

void ASOHLampSwitch::ApplyPowerState()
{
    for (ASOHLamp* Lamp : ControlledLamps)
    {
        if (!IsValid(Lamp)) continue;

        if (bPowerOn)
            Lamp->TurnOn();
        else
            Lamp->TurnOff();
    }
}

void ASOHLampSwitch::SaveState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone()) return;

    FWorldStateData& Data =
        SaveData->WorldStateMap.FindOrAdd(WorldStateID);

    Data.bIsOn = bPowerOn;
}

void ASOHLampSwitch::LoadState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone()) return;

    if (FWorldStateData* Data =
        SaveData->WorldStateMap.Find(WorldStateID))
    {
        bPowerOn = Data->bIsOn;
        ApplyPowerState(); // 🔥 중요
    }
}