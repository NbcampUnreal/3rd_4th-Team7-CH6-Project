#include "Puzzle/SOHPuzzleSaveBase.h"
#include "GameMode/SOHGameInstance.h"
#include "GameMode/SOHSaveGame.h"

ASOHPuzzleSaveBase::ASOHPuzzleSaveBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASOHPuzzleSaveBase::MarkPuzzleSolved()
{
	if (bIsSolved) return;

	bIsSolved = true;
	// ✅ 자동 저장
	if (UWorld* World = GetWorld())
	{
		if (USOHGameInstance* GI = World->GetGameInstance<USOHGameInstance>())
		{
			GI->SaveGameData();
		}
	}
}


void ASOHPuzzleSaveBase::SaveState_Implementation(USOHSaveGame* SaveData)
{
	if (!SaveData || WorldStateID.IsNone())
		return;

	FWorldStateData& Data =
		SaveData->WorldStateMap.FindOrAdd(WorldStateID);

	Data.bIsSolved = bIsSolved;
}

void ASOHPuzzleSaveBase::LoadState_Implementation(USOHSaveGame* SaveData)
{
	if (!SaveData || WorldStateID.IsNone()) return;

	if (FWorldStateData* Data = SaveData->WorldStateMap.Find(WorldStateID))
	{
		if (Data->bIsSolved)
		{
			bIsSolved = true;

			// ✅ 로드시에도 퍼즐을 "풀린 모습"으로 강제 적용
			BP_ApplySolvedState();
		}
	}
}

void ASOHPuzzleSaveBase::BP_ApplySolvedState_Implementation()
{
}