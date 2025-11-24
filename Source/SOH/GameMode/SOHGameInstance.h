#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SOHStageData.h"
#include "SOHGameInstance.generated.h"


UCLASS()
class SOH_API USOHGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USOHGameInstance();

	//현재 스테이지
	UPROPERTY(BlueprintReadWrite, Category = "Stage")
	int32 CurrentStage = 0;

	//완료조건
	UPROPERTY(BlueprintReadWrite, Category = "Stage")
	TArray<FName> CompletedConditions;

	// DataTable
	UPROPERTY(EditAnywhere , BlueprintReadOnly, Category = "Stage")
	TObjectPtr<UDataTable> StageDataTable;

public:

	UFUNCTION(BlueprintCallable, Category = "Stage")
	void CompleteCondition(FName Condition);

	// 특정 조건 완료 여부 확인
	UFUNCTION(BlueprintCallable, Category="Stage")
	bool HasCondition(FName Condition) const;

	// Stage 완료 조건 총 충족 여부
	UFUNCTION(BlueprintCallable, Category="Stage")
	bool IsStageCompleted(int32 StageID);

	// Stage를 완료하고 다음 Stage로 
	UFUNCTION(BlueprintCallable, Category="Stage")
	void AdvanceStage();
};