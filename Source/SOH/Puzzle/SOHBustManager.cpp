#include "Puzzle/SOHBustManager.h"
#include "SOHBust.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Level/SOHSlidingDoor.h"

ASOHBustManager::ASOHBustManager()
{
    PrimaryActorTick.bCanEverTick = true;

    EnterTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EnterTrigger"));
    RootComponent = EnterTrigger;
    EnterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    EnterTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    EnterTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// BeginPlay
void ASOHBustManager::BeginPlay()
{
    Super::BeginPlay();

    EnterTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASOHBustManager::OnEnterOverlap);

    if (BustPieces.Num() != TargetPositions.Num())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PuzzleManager: BustPieces(%d) �� TargetPositions(%d) ������ �ٸ��ϴ�."),
            BustPieces.Num(), TargetPositions.Num());
    }

    for (int32 i = 0; i < BustPieces.Num(); i++)
    {
        if (BustPieces[i])
        {
            BustPieces[i]->OnBustRotated.AddDynamic(this, &ASOHBustManager::CheckPuzzleSolved);
        }
    }
}

// Tick
void ASOHBustManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bPuzzleSolved)
    {
        CheckPuzzleSolved();
    }
}

void ASOHBustManager::OnEnterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor->IsA(ACharacter::StaticClass())) return;

    for (ASOHSlidingDoor* Door : TargetDoors)
    {
        if (!Door) continue;

        Door->IsArtroomPlay = true;

        Door->LockAndCloseDoor(this);
    }
}

void ASOHBustManager::CheckPuzzleSolved()
{
    if (BustPieces.Num() != TargetPositions.Num()) return;

    for (int32 i = 0; i < BustPieces.Num(); i++)
    {
        ASOHBust* Bust = BustPieces[i];
        if (!Bust) return;

        if (Bust->Position != TargetPositions[i])
        {
            return;
        }
    }

    if (!bPuzzleSolved)
    {
        bPuzzleSolved = true;

        for (ASOHBust* Bust : BustPieces)
        {
            if (Bust) Bust->bIsLocked = true;
        }

        if (SolvedSound)
            UGameplayStatics::PlaySoundAtLocation(this, SolvedSound, GetActorLocation());

        for (ASOHSlidingDoor* Door : TargetDoors)
        {
            if (Door)
            {
                Door->IsArtroomPlay = false;

                Door->UnlockSlidingDoor(this);
            }
        }
    }
}
