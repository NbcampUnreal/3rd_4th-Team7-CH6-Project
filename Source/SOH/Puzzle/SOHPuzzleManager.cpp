#include "Puzzle/SOHPuzzleManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASOHPuzzleManager::ASOHPuzzleManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASOHPuzzleManager::BeginPlay()
{
	Super::BeginPlay();
    
	if (BustPieces.Num() != 4)
	{
		UE_LOG(LogTemp, Warning, TEXT("SOHPuzzleManager: BustPieces 배열에 4개의 조각상이 있어야 합니다."));
	}
}

void ASOHPuzzleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bPuzzleSolved)
	{
		CheckPuzzleSolved();
	}
}

void ASOHPuzzleManager::CheckPuzzleSolved()
{
	if (BustPieces.Num() != 4) return;

	for (ASOHBust* Bust : BustPieces)
	{
		if (!Bust) return; // 조각상이 없으면 체크 중단

		if (Bust->Position != TargetPosition)
		{
			return; // 하나라도 목표와 다르면 퍼즐 미완료
		}
	}

	// 모두 목표 Position과 일치하면 퍼즐 완료
	bPuzzleSolved = true;
	UE_LOG(LogTemp, Warning, TEXT("Puzzle Solved!"));

	// 액터 삭제 (원하는 동작에 맞게 수정 가능)
	Destroy();
}
