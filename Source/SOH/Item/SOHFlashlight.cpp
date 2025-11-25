#include "Item/SOHFlashlight.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "SOHInventoryComponent.h"
#include "Character/SOHPlayerCharacter.h"

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
    //Spot->SetIntensity(IntensityOn);

    bOn = false;
    bEquipped = false;
}

void ASOHFlashlight::BeginPlay()
{
    Super::BeginPlay();

    SetOn(bStartOn);

    if (!bEquipped)
    {
        SetOn(false);
    }
}

void ASOHFlashlight::SetOn(bool bEnable)
{
    bOn = bEnable;

    if (!Spot) return;

    Spot->SetVisibility(bOn, true);
    Spot->SetHiddenInGame(!bOn);
    //Spot->SetIntensity(bOn ? IntensityOn : 0.f);
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