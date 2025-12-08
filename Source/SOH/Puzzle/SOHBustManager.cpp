#include "Puzzle/SOHBustManager.h"
#include "SOHBust.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// 생성자
ASOHBustManager::ASOHBustManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

// BeginPlay
void ASOHBustManager::BeginPlay()
{
    Super::BeginPlay();

    // 배열 길이 체크
    if (BustPieces.Num() != TargetPositions.Num())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PuzzleManager: BustPieces(%d) 와 TargetPositions(%d) 개수가 다릅니다."),
            BustPieces.Num(), TargetPositions.Num());
    }

    // Bust 회전 이벤트 바인딩 → 조각상이 회전될 때 자동 검사
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

// -------------------------------------------------------------
// ? 핵심 함수: 현재 상태가 TargetPositions 과 일치하는지 검사
// -------------------------------------------------------------
void ASOHBustManager::CheckPuzzleSolved()
{
    // 배열 길이 불일치 → 검사 불가
    if (BustPieces.Num() != TargetPositions.Num())
        return;

    // 각각의 조각상 Position 이 정답인지 검사
    for (int32 i = 0; i < BustPieces.Num(); i++)
    {
        ASOHBust* Bust = BustPieces[i];
        if (!Bust) return;

        // 하나라도 다르면 실패 → 검사 종료
        if (Bust->Position != TargetPositions[i])
        {
            return;
        }
    }

    // 여기 도달하면 모든 조각상 위치가 정답!
    if (!bPuzzleSolved)
    {
        bPuzzleSolved = true;

        // 조각상 모두 잠금 처리
        for (ASOHBust* Bust : BustPieces)
        {
            if (Bust) Bust->bIsLocked = true;
        }

        if (SolvedSound)
            UGameplayStatics::PlaySoundAtLocation(this, SolvedSound, GetActorLocation());
    }
}
