#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "SOHStageData.h"
#include "SOHGameInstance.generated.h"


UCLASS()
class SOH_API USOHGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USOHGameInstance();

	// 현재 스테이지
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentStage = 0;

	// 완료된 조건 GameplayTag 모음
	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer CompletedConditions;

	// Stage DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDataTable* StageDataTable;

public:
	UFUNCTION(BlueprintCallable)
	void CompleteCondition(FGameplayTag ConditionTag);

	UFUNCTION(BlueprintCallable)
	bool HasCondition(FGameplayTag ConditionTag) const;

	bool IsStageCompleted(int32 StageID);

	void AdvanceStage();

	UFUNCTION(BlueprintCallable)
	void SaveGameData();

	UFUNCTION(BlueprintCallable)
	bool LoadGameData();

	void DebugPrint();
	
	//SaveGame 로드 후 임시 저장용
	UPROPERTY()
	FTransform LoadedPlayerTransform;

	UPROPERTY()
	float LoadedHealth;

	UPROPERTY()
	float LoadedStamina;

	UPROPERTY()
	bool bLoadedFromSave = false;

	
};