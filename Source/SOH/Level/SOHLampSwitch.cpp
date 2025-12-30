#include "SOHLampSwitch.h"
#include "SOHLamp.h"
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

    // 잠금 상태 확인
    if (bIsLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LampSwitch] Switch is LOCKED - Showing message"));

        // MessageManager 찾기
        USOHMessageManager* MessageMgr = Caller->FindComponentByClass<USOHMessageManager>();

        if (MessageMgr)
        {
            // 메시지 표시 (2초간)
			FText Msg = FText::FromString(TEXT(" 불이 켜지지 않는다 "));

            MessageMgr->ShowMessageText(Msg, 2.0f);

            UE_LOG(LogTemp, Log, TEXT("[LampSwitch] Message displayed"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[LampSwitch] MessageManager NOT Found on Caller!"));
        }

        return;  // 잠겨있으면 여기서 종료
    }

    // 잠금 해제됨 → 정상 작동
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
    if (!bIsLocked)
    {
        for (ASOHLamp* Lamp : ControlledLamps)
        {
            if (IsValid(Lamp))
            {
                Lamp->ToggleLight();
            }
        }
    }
}

void ASOHLampSwitch::SetLocked(bool bNewLocked)
{
    bIsLocked = bNewLocked;
}