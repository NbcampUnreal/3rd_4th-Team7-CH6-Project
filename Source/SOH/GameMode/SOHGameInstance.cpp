#include "SOHGameInstance.h"
#include "SOHSaveGame.h"
#include "Character/SOHPlayerCharacter.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

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

void USOHGameInstance::Init()
{
    Super::Init();

    UE_LOG(LogTemp, Warning, TEXT("[GI] Init called"));

    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
        this,
        &USOHGameInstance::OnPostLoadMapWithWorld
    );
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
    // 🔥 핵심 1: SaveGame 재사용
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = Cast<USOHSaveGame>(
            UGameplayStatics::CreateSaveGameObject(USOHSaveGame::StaticClass())
        );

        if (!CurrentSaveGame)
        {
            UE_LOG(LogTemp, Error, TEXT("[SAVE] ❌ SaveGame 객체 생성 실패!"));
            return;
        }
    }

    USOHSaveGame* Save = CurrentSaveGame;

    // 스테이지 정보 저장
    Save->SavedStage      = CurrentStage;
    Save->SavedConditions = CompletedConditions;

    // 플레이어 정보 저장
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player)
    {
        Save->PlayerTransform = Player->GetActorTransform();

        if (ASOHPlayerCharacter* P = Cast<ASOHPlayerCharacter>(Player))
        {
            Save->SavedHealth   = P->GetHealth();
            Save->SavedStamina = P->GetStamina();

            if (USOHInventoryComponent* Inv = P->FindComponentByClass<USOHInventoryComponent>())
            {
                Save->SavedInventory = Inv->GetInventoryContents();
            }
        }
    }
    
    // 월드 액터 저장
    TArray<AActor*> SaveActors;
    UGameplayStatics::GetAllActorsWithInterface(
        GetWorld(),
        USOHSaveObjectInterface::StaticClass(),
        SaveActors
    );

    UE_LOG(LogTemp, Warning, TEXT("[SAVE] 🌍 SaveObjectInterface 액터 수: %d"), SaveActors.Num());

    for (AActor* Actor : SaveActors)
    {
        ISOHSaveObjectInterface::Execute_SaveState(Actor, Save);
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[SAVE] WorldStateMap.Num=%d"),
        Save->WorldStateMap.Num()
    );

    // 실제 슬롯 저장
    if (UGameplayStatics::SaveGameToSlot(Save, SAVE_SLOT_NAME, SAVE_USER_INDEX))
    {
        UE_LOG(LogTemp, Warning, TEXT("[SAVE] ✅ 저장 완료!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SAVE] ❌ 저장 실패!"));
    }
}


bool USOHGameInstance::LoadGameData()
{
    if (!UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, SAVE_USER_INDEX))
        return false;

    CurrentSaveGame = Cast<USOHSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SAVE_SLOT_NAME, SAVE_USER_INDEX)
    );

    if (!CurrentSaveGame)
        return false;

    CurrentStage        = CurrentSaveGame->SavedStage;
    CompletedConditions = CurrentSaveGame->SavedConditions;

    LoadedPlayerTransform = CurrentSaveGame->PlayerTransform;
    LoadedHealth          = CurrentSaveGame->SavedHealth;
    LoadedStamina         = CurrentSaveGame->SavedStamina;
    LoadedInventory       = CurrentSaveGame->SavedInventory;

    bLoadedFromSave = true;

    UE_LOG(LogTemp, Warning, TEXT("[LOAD] ✅ SaveGame 로드 완료 (WorldState=%d)"),
        CurrentSaveGame->WorldStateMap.Num());

    return true;
}


bool USOHGameInstance::HasSaveFile() const
{
    return UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, SAVE_USER_INDEX);
}

void USOHGameInstance::ResetGameData()
{
    CurrentStage = 0;
    CompletedConditions.Reset();

    bLoadedFromSave = false;

    LoadedPlayerTransform = FTransform::Identity;
    LoadedHealth = 200.0f;
    LoadedStamina = 100.0f;
    LoadedInventory.Empty();

    CurrentSaveGame = nullptr;

    // ⭐ 세이브 파일 삭제
    UGameplayStatics::DeleteGameInSlot(SAVE_SLOT_NAME, SAVE_USER_INDEX);
}
void USOHGameInstance::ContinueGame()
{
    if (!LoadGameData())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 로드 실패"));
        return;
    }

    // ✅ 이번 레벨 로드가 '컨티뉴 로드'임을 표시
    bPendingApplyWorldState = true;
    
    UE_LOG(LogTemp, Error, TEXT("🔥 CALL ApplyWorldState FROM ContinueGame"));
    ApplyWorldState();
    
    UGameplayStatics::OpenLevel(this, GameLevelName);
}

void USOHGameInstance::ApplyWorldState()
{
    if (!CurrentSaveGame)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LOAD] No SaveGame → Skip Apply"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithInterface(
        World,
        USOHSaveObjectInterface::StaticClass(),
        FoundActors
    );

    UE_LOG(LogTemp, Warning,
        TEXT("[LOAD] 🌍 ApplyWorldState - 액터 수: %d"),
        FoundActors.Num()
    );

    for (AActor* Actor : FoundActors)
    {
        ISOHSaveObjectInterface::Execute_LoadState(
            Actor,
            CurrentSaveGame
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("[LOAD] ✅ ApplyWorldState 완료"));
}

void USOHGameInstance::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
    UE_LOG(LogTemp, Warning, TEXT("[GI] OnPostLoadMapWithWorld called"));

    if (!bPendingApplyWorldState)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GI] Not a continue-load. Skip Apply."));
        return;
    }

    // ❌ 여기서 Save 준비 체크하지 않는다

    // 한 번만 적용하도록 플래그 리셋
    bPendingApplyWorldState = false;

    LoadedWorld->GetTimerManager().SetTimerForNextTick([this]()
    {
        UE_LOG(LogTemp, Warning, TEXT("[GI] Delayed ApplyWorldState"));

        // ✅ 실제 적용 직전에 체크
        if (!bLoadedFromSave || !CurrentSaveGame)
        {
            UE_LOG(LogTemp, Warning, TEXT("[GI] Save still not ready. Abort Apply."));
            return;
        }

        ApplyWorldState();
    });
}
