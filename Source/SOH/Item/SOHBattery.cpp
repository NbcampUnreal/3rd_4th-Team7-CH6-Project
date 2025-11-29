#include "SOHBattery.h"
#include "SOHFlashlight.h"
#include "Character/SOHPlayerCharacter.h"
#include "SOHInventoryComponent.h"

ASOHBattery::ASOHBattery()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASOHBattery::Interact_Implementation(AActor* Caller)
{
    if (!Caller) return;

    USOHInventoryComponent* InventoryComp = Caller->FindComponentByClass<USOHInventoryComponent>();
    const bool bAdded = InventoryComp ? InventoryComp->AddToInventory(itemID, 1) : false;

    if (!bAdded) return;

    Destroy();
}

bool ASOHBattery::Use(AActor* User, const FSOHItemTableRow& ItemData)
{
    if (!User) return false;

    ASOHPlayerCharacter* PC = Cast<ASOHPlayerCharacter>(User);
    if (!PC) return false;

    ASOHFlashlight* Flashlight = PC->GetFlashlight();
    if (!Flashlight) return false;

    float ChargeAmount = ItemData.value;

    bool bCharged = Flashlight->UseBatteryItem(ChargeAmount);
    return bCharged;
}