#include "SOHGameInstance.h"
#include "SOHSaveGame.h"
#include "Character/SOHPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// 세이브 슬롯 상수 정의
const FString USOHGameInstance::SAVE_SLOT_NAME = TEXT("SOH_SaveSlot");
const int32 USOHGameInstance::SAVE_USER_INDEX = 0;

USOHGameInstance::USOHGameInstance()
{
    // 기본값 초기화
    CurrentStage = 0;
    LoadedHealth = 200.0f;
    LoadedStamina = 100.0f;
    bLoadedFromSave = false;
}

void USOHGameInstance::CompleteCondition(FGameplayTag ConditionTag)
{
    // None 태그 방어
    if (!ConditionTag.IsValid() || ConditionTag.ToString() == "None")
    {
        UE_LOG(LogTemp, Error, TEXT("[Condition] ❌ None 태그로 호출됨! 무시됨"));
        return;
    }

    // 이미 완료된 조건인지 확인
    if (CompletedConditions.HasTagExact(ConditionTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Condition] ⚠️ 이미 완료됨: %s (재브로드캐스트)"), 
               *ConditionTag.ToString());
        
        // 이미 완료된 조건도 Broadcast
        OnConditionCompleted.Broadcast(ConditionTag);
        return;
    }

    // 조건 완료 처리
    CompletedConditions.AddTag(ConditionTag);
    UE_LOG(LogTemp, Warning, TEXT("[Condition] ✅ 완료: %s (총 %d개 완료)"), 
           *ConditionTag.ToString(), 
           CompletedConditions.Num());

    // 델리게이트 브로드캐스트
    OnConditionCompleted.Broadcast(ConditionTag);
    
    // 스테이지 완료 여부 체크
    if (IsStageCompleted(CurrentStage))
    {
        AdvanceStage();
    }
}

bool USOHGameInstance::HasCondition(FGameplayTag ConditionTag) const
{
    return CompletedConditions.HasTagExact(ConditionTag);
}

bool USOHGameInstance::IsStageCompleted(int32 StageID)
{
    if (!StageDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[Stage] ❌ StageDataTable이 설정되지 않았습니다!"));
        return false;
    }

    // DataTable에서 스테이지 데이터 가져오기
    FName RowName(*FString::FromInt(StageID));
    FSOHStageData* Row = StageDataTable->FindRow<FSOHStageData>(RowName, TEXT(""));

    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Stage] ⚠️ Stage %d 데이터를 찾을 수 없습니다!"), StageID);
        return false;
    }

    // 필요한 모든 조건이 완료되었는지 확인
    bool bCompleted = CompletedConditions.HasAll(Row->RequiredConditions);
    
    if (bCompleted)
    {
        UE_LOG(LogTemp, Log, TEXT("[Stage] ✅ Stage %d 완료 조건 충족!"), StageID);
    }

    return bCompleted;
}

void USOHGameInstance::AdvanceStage()
{
    UE_LOG(LogTemp, Warning, TEXT("===================================="));
    UE_LOG(LogTemp, Warning, TEXT("📖 Stage %d 완료! 다음 Stage로 이동"), CurrentStage);
    UE_LOG(LogTemp, Warning, TEXT("===================================="));
    
    CurrentStage++;
    
    // 스테이지 진행 시 자동 저장
    SaveGameData();
    
    UE_LOG(LogTemp, Warning, TEXT("[AUTO SAVE] 💾 Stage 변경으로 자동 저장 완료"));
}

//====================================
// SaveGame System
//====================================

void USOHGameInstance::SaveGameData()
{
    // SaveGame 객체 생성
    USOHSaveGame* Save = Cast<USOHSaveGame>(
        UGameplayStatics::CreateSaveGameObject(USOHSaveGame::StaticClass())
    );

    if (!Save)
    {
        UE_LOG(LogTemp, Error, TEXT("[SAVE] ❌ SaveGame 객체 생성 실패!"));
        return;
    }

    // 스테이지 정보 저장
    Save->SavedStage = CurrentStage;
    Save->SavedConditions = CompletedConditions;

    // 플레이어 정보 저장
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player)
    {
        // 현재 위치 저장
        Save->PlayerTransform = Player->GetActorTransform();

        // 플레이어 스탯 저장
        ASOHPlayerCharacter* P = Cast<ASOHPlayerCharacter>(Player);
        if (P)
        {
            Save->SavedHealth = P->GetHealth();
            Save->SavedStamina = P->GetStamina();
            
            // 인벤토리 저장
            if (USOHInventoryComponent* Inv = P->FindComponentByClass<USOHInventoryComponent>())
            {
                Save->SavedInventory = Inv->GetInventoryContents();
            }

            UE_LOG(LogTemp, Log, TEXT("[SAVE] 플레이어 상태: HP=%.0f, Stamina=%.0f, 위치=%s"), 
                   Save->SavedHealth, 
                   Save->SavedStamina,
                   *Save->PlayerTransform.GetLocation().ToString());
        }
    }
    
    // ⭐⭐⭐ 여기에 추가!
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[SAVE] 🌍 월드 액터 저장 (GameplayTag 기반)"));
    UE_LOG(LogTemp, Log, TEXT("[SAVE] 완료된 조건: %d개"), CompletedConditions.Num());
    
    // CompletedConditions를 문자열로 변환해서 저장
    FString TagsString;
    for (const FGameplayTag& Tag : CompletedConditions)
    {
        if (!TagsString.IsEmpty())
        {
            TagsString += TEXT(",");
        }
        TagsString += Tag.ToString();
        UE_LOG(LogTemp, Log, TEXT("[SAVE]   📦 %s"), *Tag.ToString());
    }
    
    // ⭐ SaveGame에 추가 (일단 간단하게)
    Save->SavedConditions = CompletedConditions;
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    if (UGameplayStatics::SaveGameToSlot(Save, SAVE_SLOT_NAME, SAVE_USER_INDEX))
    {
        UE_LOG(LogTemp, Warning, TEXT("[SAVE] ✅ Stage %d 저장 완료!"), CurrentStage);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SAVE] ❌ 저장 실패!"));
    }
}

bool USOHGameInstance::LoadGameData()
{
    // 세이브 파일 존재 여부 확인
    if (!UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, SAVE_USER_INDEX))
    {
        UE_LOG(LogTemp, Warning, TEXT("[LOAD] ⚠️ 세이브 파일이 없습니다."));
        return false;
    }

    // SaveGame 로드
    USOHSaveGame* Save = Cast<USOHSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SAVE_SLOT_NAME, SAVE_USER_INDEX)
    );
    
    if (!Save)
    {
        UE_LOG(LogTemp, Error, TEXT("[LOAD] ❌ 세이브 파일 로드 실패!"));
        return false;
    }

    // 스테이지 정보 복원
    CurrentStage = Save->SavedStage;
    CompletedConditions = Save->SavedConditions;

    // 플레이어 데이터 임시 저장 (나중에 ApplyLoadedData에서 사용)
    LoadedPlayerTransform = Save->PlayerTransform;
    LoadedHealth = Save->SavedHealth;
    LoadedStamina = Save->SavedStamina;
    LoadedInventory = Save->SavedInventory;

    // 로드 플래그 설정
    bLoadedFromSave = true;

    UE_LOG(LogTemp, Warning, TEXT("[LOAD] ✅ 로드 완료!"));
    UE_LOG(LogTemp, Log, TEXT("[LOAD] Stage: %d"), CurrentStage);
    UE_LOG(LogTemp, Log, TEXT("[LOAD] HP: %.0f, Stamina: %.0f"), LoadedHealth, LoadedStamina);
    UE_LOG(LogTemp, Log, TEXT("[LOAD] 위치: %s"), *LoadedPlayerTransform.GetLocation().ToString());
    UE_LOG(LogTemp, Log, TEXT("[LOAD] 완료된 조건: %d개"), CompletedConditions.Num());
    
    UE_LOG(LogTemp, Warning, TEXT("[LOAD] 🌍 완료된 조건: %d개"), CompletedConditions.Num());
    for (const FGameplayTag& Tag : CompletedConditions)
    {
        UE_LOG(LogTemp, Log, TEXT("[LOAD]   📦 %s"), *Tag.ToString());
    }
    
    return true;
}

bool USOHGameInstance::HasSaveFile() const
{
    return UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, SAVE_USER_INDEX);
}

void USOHGameInstance::ResetGameData()
{
    UE_LOG(LogTemp, Log, TEXT("[RESET] 🔄 새 게임 데이터 초기화"));

    // 스테이지 초기화
    CurrentStage = 0;
    CompletedConditions.Reset();
    
    // 로드 플래그 초기화
    bLoadedFromSave = false;
    
    // 플레이어 데이터 초기화
    LoadedPlayerTransform = FTransform::Identity;
    LoadedHealth = 200.0f;      // 새 게임 기본 체력
    LoadedStamina = 100.0f;     // 새 게임 기본 스태미나
    LoadedInventory.Empty();
    
    UE_LOG(LogTemp, Log, TEXT("[RESET] ✅ 초기화 완료"));
}

void USOHGameInstance::ContinueGame()
{
    if (!LoadGameData())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 로드 실패"));
        return;
    }
    
    FString CurrentLevel = GetWorld()->GetMapName();
    
    if (CurrentLevel.Contains("MainMenu") || CurrentLevel.Contains(LobbyLevelName.ToString()))
    {
        // ⭐ 메인 메뉴 → 게임 레벨
        UGameplayStatics::OpenLevel(this, GameLevelName);
    }
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        ASOHPlayerCharacter* PlayerCharacter = Cast<ASOHPlayerCharacter>(PC->GetPawn());
        if (PlayerCharacter)
        {
            PlayerCharacter->ApplyLoadedData();
        }
    }
}