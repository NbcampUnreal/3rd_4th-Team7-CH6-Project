#include "Item/SOHFlashlight.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "SOHInventoryComponent.h"
#include "Character/SOHPlayerCharacter.h"
#include "UI/SOHMessageManager.h"
#include "Level/SOHJumpScareBase.h"

ASOHFlashlight::ASOHFlashlight()
{
    PrimaryActorTick.bCanEverTick = false;

    Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
    RootComponent = Pivot;

    if (itemMesh)
    {
        itemMesh->SetupAttachment(Pivot);

        itemMesh->SetSimulatePhysics(false);
        itemMesh->SetEnableGravity(false);

        itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        itemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }

    Spot = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot"));
    Spot->SetupAttachment(Pivot);
    Spot->bUseInverseSquaredFalloff = false;

    bOn = false;
    bEquipped = false;
}

void ASOHFlashlight::BeginPlay()
{
    Super::BeginPlay();

    CurrentBattery = FMath::Clamp(InitialBattery, 0.f, MaxBattery);

    SetOn(bStartOn);

    if (!bEquipped)
    {
        SetOn(false);
    }
}

void ASOHFlashlight::SetOn(bool bEnable)
{
    if (bEnable && IsBatteryEmpty())
        bEnable = false;

    bOn = bEnable;

    if (!Spot) return;

    Spot->SetVisibility(bOn, true);
    Spot->SetHiddenInGame(!bOn);

    UpdateLightFromBattery();

    if (bOn) StartBatteryDrain();
    else     StopBatteryDrain();
}

void ASOHFlashlight::Toggle()
{
    if (!bEquipped) return;

    SetOn(!bOn);
}

void ASOHFlashlight::SetEquipped(ACharacter* NewOwner)
{
    if (!NewOwner) return;

    OwnerChar = NewOwner;
    OwnerMesh = NewOwner->GetMesh();
    if (!OwnerMesh || !OwnerMesh->DoesSocketExist(HandSocketName)) return;

    if (itemMesh)
    {
        itemMesh->SetSimulatePhysics(false);
        itemMesh->SetEnableGravity(false);
        itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    AttachToComponent(
        OwnerMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        HandSocketName
    );

    bEquipped = true;

    SetOn(false);

    SetActorEnableCollision(false);
    HideInteractWidget();
    ApplyOverlayMaterial(nullptr);
}

void ASOHFlashlight::StartBatteryDrain()
{
    if (MaxBattery <= 0.f) return;

    if (DrainInterval <= 0.f)
        DrainInterval = 1.f;

    if (GetWorldTimerManager().IsTimerActive(BatteryDrainTimer)) return;

    GetWorldTimerManager().SetTimer(
        BatteryDrainTimer,
        this,
        &ASOHFlashlight::DrainOnce,
        DrainInterval,
        true
    );
}

void ASOHFlashlight::StopBatteryDrain()
{
    GetWorldTimerManager().ClearTimer(BatteryDrainTimer);
}

void ASOHFlashlight::DrainOnce()
{
    if (MaxBattery <= 0.f)
    {
        StopBatteryDrain();
        return;
    }

    CurrentBattery = FMath::Clamp(CurrentBattery - DrainRate, 0.f, MaxBattery);

    if (CurrentBattery <= 0.f)
    {
        CurrentBattery = 0.f;
        SetOn(false);
        StopBatteryDrain();

        if (OwnerChar)
        {
            if (USOHMessageManager* MsgMgr = OwnerChar->FindComponentByClass<USOHMessageManager>())
            {
                MsgMgr->ShowMessageText(
                    FText::FromString(TEXT("비상 배터리 방전 비상")),
                    1.5f
                );
            }
        }

        return;
    }
    UpdateLightFromBattery();
}

void ASOHFlashlight::UpdateLightFromBattery()
{
    if (!Spot)
        return;

    const float Ratio = (MaxBattery <= 0.f) ? 0.f : CurrentBattery / MaxBattery;
}

bool ASOHFlashlight::UseBatteryItem(float ChargeAmount)
{
    if (ChargeAmount <= 0.f || MaxBattery <= 0.f)
        return false;

    float Before = CurrentBattery;
    const bool bWasEmpty = (CurrentBattery <= 0.f);

    CurrentBattery = FMath::Clamp(CurrentBattery + ChargeAmount, 0.f, MaxBattery);

    UpdateLightFromBattery();

    if (CurrentBattery > Before)
    {
        if (bEquipped && !bOn && CurrentBattery > 0.f)
        {
            SetOn(true);
        }
        else if (bOn && bWasEmpty && CurrentBattery > 0.f)
        {
            StartBatteryDrain();
        }
    }

    return CurrentBattery > Before;
}

float ASOHFlashlight::GetBatteryPercent() const
{
    if (MaxBattery <= 0.f)
        return 0.f;

    return FMath::Clamp(CurrentBattery / MaxBattery, 0.f, 1.f);
}

void ASOHFlashlight::Interact_Implementation(AActor* Caller)
{
    if (!Caller) return;

    USOHInventoryComponent* InventoryComp = Caller->FindComponentByClass<USOHInventoryComponent>();
    const bool bAdded = InventoryComp ? InventoryComp->AddToInventory(itemID, 1) : false;

    if (!bAdded)
        return;

    if (ACharacter* Char = Cast<ACharacter>(Caller))
    {
        SetEquipped(Char);

        if (ASOHPlayerCharacter* PC = Cast<ASOHPlayerCharacter>(Caller))
        {
            PC->SetFlashlight(this);
        }
    }
}