#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SOHStageData.generated.h"

// DataTable에서 한 Row = 하나의 Stage 설정값
USTRUCT(BlueprintType)
struct FSOHStageData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Stage 이름(옵션)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StageName;

	// 이 Stage를 완료하기 위한 조건 리스트
	// 예: ["KeyPicked", "DoorOpened"]
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredConditions;
};
