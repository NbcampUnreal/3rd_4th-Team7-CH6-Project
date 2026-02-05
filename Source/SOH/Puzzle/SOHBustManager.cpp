#include "Puzzle/SOHBustManager.h"
#include "SOHBust.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "GameMode/SOHGameInstance.h"
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
    if (!OtherActor || !OtherActor->IsA(ACharacter::StaticClass())) return;
    
    OverlappedComp->SetGenerateOverlapEvents(false);
    OverlappedComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    if (IsValid(EnterCutscenePlayer))
    {
        UE_LOG(LogTemp, Warning, TEXT("[CUTSCENE] EnterOverlap -> PlayCutscene"));
        EnterCutscenePlayer->PlayCutscene();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[CUTSCENE] EnterCutscenePlayer is NULL"));
    }
    
    for (ASOHSlidingDoor* Door : TargetDoors)
    {
        if (!Door) continue;

        Door->IsArtroomPlay = true;
        Door->LockAndCloseDoor(this);
    }
    
    if (USOHGameInstance* GI = GetWorld()->GetGameInstance<USOHGameInstance>())
    {
        GI->SaveGameData();
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

        if (PuzzleClearTag.IsValid())
        {
            if (USOHGameInstance* GI = GetWorld()->GetGameInstance<USOHGameInstance>())
            {
                GI->CompleteCondition(PuzzleClearTag);
                GI->SaveGameData();
                UE_LOG(LogTemp, Warning, TEXT("Combination Puzzle Completed! Tag = %s"), *PuzzleClearTag.ToString());
            }
        }
        
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

void ASOHBustManager::SaveState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone())
        return;

    FWorldStateData& Data =
        SaveData->WorldStateMap.FindOrAdd(WorldStateID);

    Data.bIsSolved = bPuzzleSolved;
    Data.bIsLocked = bEnterTriggered; // 트리거 상태로 재활용
}

void ASOHBustManager::LoadState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone())
        return;

    if (FWorldStateData* Data =
        SaveData->WorldStateMap.Find(WorldStateID))
    {
        bPuzzleSolved   = Data->bIsSolved;
        bEnterTriggered = Data->bIsLocked;

        // 퍼즐이 이미 풀린 상태
        if (bPuzzleSolved)
        {
            for (ASOHBust* Bust : BustPieces)
            {
                if (Bust) Bust->bIsLocked = true;
            }

            for (ASOHSlidingDoor* Door : TargetDoors)
            {
                if (Door)
                {
                    Door->IsArtroomPlay = false;
                    Door->UnlockSlidingDoor(this);
                }
            }

            EnterTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            EnterTrigger->SetGenerateOverlapEvents(false);
        }
        // 퍼즐은 안 풀렸지만 입장 트리거는 발동된 상태
        else if (bEnterTriggered)
        {
            for (ASOHSlidingDoor* Door : TargetDoors)
            {
                if (Door)
                {
                    Door->IsArtroomPlay = true;
                    Door->LockAndCloseDoor(this);
                }
            }

            EnterTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            EnterTrigger->SetGenerateOverlapEvents(false);
        }
    }
}
