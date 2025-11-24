#include "SOHGameInstance.h"
#include "Kismet/GameplayStatics.h"

USOHGameInstance::USOHGameInstance()
{
	//초기값
}

void USOHGameInstance::CompleteCondition(FName Condition)
{
	// 이미 완료한 조건인지 체크
	if (CompletedConditions.Contains(Condition))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Condition] 이미 완료됨: %s"), *Condition.ToString());
		return;
	}

	// 새 조건 추가
	CompletedConditions.Add(Condition);
	UE_LOG(LogTemp, Log, TEXT("[Condition] 완료: %s"), *Condition.ToString());

	// 현재 Stage 완료 체크
	if (IsStageCompleted(CurrentStage))
	{
		AdvanceStage();
	}
}

bool USOHGameInstance::HasCondition(FName Condition) const
{
	return CompletedConditions.Contains(Condition);
}

bool USOHGameInstance::IsStageCompleted(int32 StageID)
{
	if (!StageDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("StageDataTable이 GameInstance에 설정되지 않음!"));
		return false;
	}

	FName RowName = FName(*FString::FromInt(StageID));

	FSOHStageData* Row = StageDataTable->FindRow<FSOHStageData>(RowName, TEXT(""));

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stage %d 를 찾을 수 없음!"), StageID);
		return false;
	}

	// RequiredConditions 모두 CompletedConditions 안에 있어야 완료
	for (const FName& Needed : Row->RequiredConditions)
	{
		if (!CompletedConditions.Contains(Needed))
		{
			return false; // 하나라도 없으면 아직 미완료
		}
	}

	return true; // 모든 조건 완료!
}

void USOHGameInstance::AdvanceStage()
{
	UE_LOG(LogTemp, Warning, TEXT("==== Stage %d 완료! 다음 Stage로 이동 ===="), CurrentStage);

	CurrentStage++;
}