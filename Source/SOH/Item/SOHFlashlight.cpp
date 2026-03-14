#include "Item/SOHFlashlight.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "SOHInventoryComponent.h"
#include "Character/SOHPlayerCharacter.h"
#include "UI/SOHMessageManager.h"
#include "Level/SOHJumpScareBase.h"
#include "Item/SOHItemManager.h"
#include "GameMode/SOHSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

ASOHFlashlight::ASOHFlashlight()
{
    PrimaryActorTick.bCanEverTick = true;

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

    // 상태가 실제로 변경될 때만 사운드 재생
    bool bStateChanged = (bOn != bEnable);

    bOn = bEnable;

    if (!Spot) return;

    Spot->SetVisibility(bOn, true);
    Spot->SetHiddenInGame(!bOn);

    UpdateLightFromBattery();

    // 상태 변경 시 사운드 재생
    if (bStateChanged && bEquipped)
    {
        if (bOn && FlashlightOnSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, FlashlightOnSound, GetActorLocation());
        }
        else if (!bOn && FlashlightOffSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, FlashlightOffSound, GetActorLocation());
        }
    }

    if (bOn) StartBatteryDrain();
    else     StopBatteryDrain();
}

void ASOHFlashlight::Toggle()
{
    if (!bEquipped) return;

    SetOn(!bOn);
}

void ASOHFlashlight::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bOn && OwnerCamera && Spot)
    {
        // SpotLight만 카메라 방향으로 회전
        FRotator CameraRotation = OwnerCamera->GetComponentRotation();
        Spot->SetWorldRotation(CameraRotation);
    }
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
        itemMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
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
    SetFlashlightCutScene();

    // 카메라 참조 저장
    OwnerCamera = NewOwner->FindComponentByClass<UCameraComponent>();

    // Tick 활성화
    PrimaryActorTick.bCanEverTick = true;
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
                    FText::FromString(TEXT("배터리가 방전되었어.")),
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
    // 향후 Ratio에 따라 Spot 강도/범위 등을 조절할 수 있습니다.
}

bool ASOHFlashlight::UseBatteryItem(float ChargeAmount)
{
    if (ChargeAmount <= 0.f || MaxBattery <= 0.f)
        return false;

    float Before = CurrentBattery;

    CurrentBattery = FMath::Clamp(CurrentBattery + ChargeAmount, 0.f, MaxBattery);

    if (BatteryChargeSound)
    {
        UGameplayStatics::PlaySound2D(this, BatteryChargeSound);
    }

    UpdateLightFromBattery();

    return CurrentBattery > Before;
}

float ASOHFlashlight::GetBatteryPercent() const
{
    if (MaxBattery <= 0.f)
        return 0.f;

    return FMath::Clamp(CurrentBattery / MaxBattery, 0.f, 1.f);
}

bool ASOHFlashlight::IsOn() const
{
    return bOn;
}

void ASOHFlashlight::Interact_Implementation(AActor* Caller)
{ 
    if (!Caller) return;

    USOHInventoryComponent* InventoryComp = Caller->FindComponentByClass<USOHInventoryComponent>();
    const bool bAdded = InventoryComp ? InventoryComp->AddToInventory(itemID, 1) : false;

    if (!bAdded)
        return;
    
    UGameInstance* GI = GetGameInstance();
    USOHItemManager* ItemMgr = GI ? GI->GetSubsystem<USOHItemManager>() : nullptr;

    FText ItemName = FText::FromName(itemID);

    if (ItemMgr)
    {
        if (FSOHItemTableRow* ItemData = ItemMgr->GetItemDataByID(itemID))
        {
            ItemName = ItemData->itemName;
        }
    }
    
    if (USOHMessageManager* MsgMgr = Caller->FindComponentByClass<USOHMessageManager>())
    {
        FText Msg = FText::Format(
            FText::FromString(TEXT("{0}을(를) 획득했다.")),
            ItemName
        );
        MsgMgr->ShowMessageText(Msg, 1.5f);
    }
    
    TryTriggerItemCutscene();
    
    if (ACharacter* Char = Cast<ACharacter>(Caller))
    {
        SetEquipped(Char);

        if (ASOHPlayerCharacter* PC = Cast<ASOHPlayerCharacter>(Caller))
        {
            PC->SetFlashlight(this);
        }
    }
}

void ASOHFlashlight::SetFlashlightCutScene()
{
    PlayFlashlightCutScene();
}

void ASOHFlashlight::SaveState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone())
        return;

    // 손에 장착된 상태라면 저장
    if (!bEquipped)
        return;

    FWorldStateData& Data =
        SaveData->WorldStateMap.FindOrAdd(WorldStateID);

    // 기존 동작 유지: 장착 상태를 기록 (퍼즐/월드 상태 용도)
    Data.bIsSolved = true;

    // 배터리 관련 정보 저장
    Data.bIsOn = bOn;
    Data.BatteryLevel = CurrentBattery;

    UE_LOG(LogTemp, Warning,
        TEXT("[SAVE][Flashlight] %s equipped"),
        *WorldStateID.ToString());
}

void ASOHFlashlight::LoadState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone())
        return;

    FWorldStateData* Data =
        SaveData->WorldStateMap.Find(WorldStateID);

    if (!Data || !Data->bIsSolved)
        return;

    UE_LOG(LogTemp, Warning,
        TEXT("[LOAD][Flashlight] Restore %s"),
        *WorldStateID.ToString());

    ACharacter* Player =
        UGameplayStatics::GetPlayerCharacter(this, 0);

    if (!Player)
        return;

    // 다시 장착
    SetEquipped(Player);

    // 배터리 잔량 복원
    CurrentBattery = FMath::Clamp(Data->BatteryLevel, 0.f, MaxBattery);

    // 화면/소유자에 알리기 위해 상태 동기화
    if (ASOHPlayerCharacter* PC = Cast<ASOHPlayerCharacter>(Player))
    {
        PC->SetFlashlight(this);
    }
}