#include "SOHWarpMirror.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

ASOHWarpMirror::ASOHWarpMirror()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    WarpTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("WarpTrigger"));
    WarpTrigger->SetupAttachment(RootComponent);
    WarpTrigger->SetBoxExtent(FVector(50.f, 50.f, 100.f));
    WarpTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    WarpTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    WarpTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    WarpTrigger->SetGenerateOverlapEvents(true);

    DestinationPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DestinationPoint"));
    DestinationPoint->SetupAttachment(RootComponent);
    DestinationPoint->SetRelativeLocation(FVector(100.f, 0.f, 0.f));

    bCanWarp = true;
}

void ASOHWarpMirror::BeginPlay()
{
    Super::BeginPlay();

    if (WarpTrigger)
    {
        WarpTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASOHWarpMirror::OnOverlapBegin);
    }
}

void ASOHWarpMirror::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!bCanWarp)
    {
        return;
    }

    if (!TargetWarp)
    {
        return;
    }

    if (!OtherActor)
    {
        return;
    }

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar)
    {
        return;
    }

    bCanWarp = false;

    const FVector DestLocation = TargetWarp->DestinationPoint->GetComponentLocation();
    const FRotator DestRotation = TargetWarp->DestinationPoint->GetComponentRotation();

    PlayerChar->SetActorLocationAndRotation(DestLocation, DestRotation, false, nullptr, ETeleportType::TeleportPhysics);

    TargetWarp->bCanWarp = false;

    const float WarpCooldown = 0.3f;
    GetWorldTimerManager().SetTimer(TimerHandle_ResetWarp, this, &ASOHWarpMirror::ResetWarp, WarpCooldown, false);
}

void ASOHWarpMirror::ResetWarp()
{
    bCanWarp = true;

    if (TargetWarp)
    {
        TargetWarp->bCanWarp = true;
    }
}