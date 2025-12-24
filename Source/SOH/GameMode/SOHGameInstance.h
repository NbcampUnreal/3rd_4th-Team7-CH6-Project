#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "Item/SOHInventoryComponent.h"
#include "SOHStageData.h"
#include "SOHGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConditionCompleted, FGameplayTag, CompletedTag);

UCLASS()
class SOH_API USOHGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    USOHGameInstance();
    
    // 현재 스테이지 ID
    UPROPERTY(BlueprintReadOnly, Category = "Stage")
    int32 CurrentStage = 0;

    // 완료된 조건들
    UPROPERTY(BlueprintReadOnly, Category = "Stage")
    FGameplayTagContainer CompletedConditions;

    // Stage 설정 DataTable
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage")
    UDataTable* StageDataTable;

    // 조건 완료 시 브로드캐스트
    UPROPERTY(BlueprintAssignable, Category = "Stage")
    FOnConditionCompleted OnConditionCompleted;

    // 조건 완료 처리
    UFUNCTION(BlueprintCallable, Category = "Stage")
    void CompleteCondition(FGameplayTag ConditionTag);

    // 조건 완료 여부 확인
    UFUNCTION(BlueprintPure, Category = "Stage")
    bool HasCondition(FGameplayTag ConditionTag) const;

    // 스테이지 완료 여부 확인
    bool IsStageCompleted(int32 StageID);

    // 다음 스테이지로 진행
    void AdvanceStage();
    
    // 세이브 슬롯 상수
    static const FString SAVE_SLOT_NAME;
    static const int32 SAVE_USER_INDEX;

    // 게임 저장
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void SaveGameData();

    // 게임 로드
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    bool LoadGameData();

    // 세이브 파일 존재 여부
    UFUNCTION(BlueprintPure, Category = "SaveGame")
    bool HasSaveFile() const;

    // 새 게임 데이터 초기화
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void ResetGameData();
    
    UFUNCTION(BlueprintCallable, Category = "Game")
    void ContinueGame();
    
    // 로드 플래그
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    bool bLoadedFromSave = false;

    // 로드된 플레이어 위치
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    FTransform LoadedPlayerTransform;

    // 로드된 플레이어 체력
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    float LoadedHealth = 200.0f;

    // 로드된 플레이어 스태미나
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    float LoadedStamina = 100.0f;

    // 로드된 인벤토리
    UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
    TArray<FSOHInventoryItem> LoadedInventory;
	
    UPROPERTY(EditDefaultsOnly, Category="Level")
    FName LobbyLevelName = "MainMenuLevel";

    UPROPERTY(EditDefaultsOnly, Category="Level")
    FName GameLevelName = "MainLevel";
};