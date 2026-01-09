#include "Puzzle/SOHPuzzleSaveBase.h"
#include "GameMode/SOHGameInstance.h"
#include "GameMode/SOHSaveGame.h"

ASOHPuzzleSaveBase::ASOHPuzzleSaveBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASOHPuzzleSaveBase::MarkPuzzleSolved()
{
	if (bIsSolved)
		return;

	bIsSolved = true;
	
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
	if (!SaveData || WorldStateID.IsNone())
		return;

	if (FWorldStateData* Data =
		SaveData->WorldStateMap.Find(WorldStateID))
	{
		if (Data->bIsSolved)
		{
			bIsSolved = true;
			// 이미 해결된 퍼즐이면 제거 (필요 시 BP에서 오버라이드 가능)
		}
	}
}
