#include "Item/SOHBaseItem.h" // 경로 확인
#include "SOHItemManager.h"
#include "SOHInventoryComponent.h" // 인벤토리 컴포넌트
#include "Components/StaticMeshComponent.h"
#include "GameMode/SOHGameInstance.h"
#include "GameMode/SOHCutscenePlayerBase.h"
#include "GameMode/SOHSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SOHMessageManager.h"

// 커스텀 로그 카테고리 정의
// Output Log 필터에서 'SOHItem'으로 검색하거나 카테고리를 선택할 수 있습니다.
DEFINE_LOG_CATEGORY_STATIC(LogSOHItem, Log, All);

ASOHBaseItem::ASOHBaseItem()
{
    // 1. 외형을 담당할 스태틱 메쉬 컴포넌트 생성
    itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = itemMesh;

    if (InteractionRange)
        InteractionRange->SetupAttachment(itemMesh);

    if (InteractionWidget)
        InteractionWidget->SetupAttachment(itemMesh);

    // 2. 물리 및 충돌 설정
    itemMesh->SetSimulatePhysics(true);
    itemMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    itemMesh->SetGenerateOverlapEvents(true);
    
    // 상호작용(E키, 라인트레이스)을 위해 Visibility 채널 Block 설정
    itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    itemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    itemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // 아이템은 굳이 틱이 필요 없으므로 꺼둠 (최적화)
    PrimaryActorTick.bCanEverTick = false;
}

void ASOHBaseItem::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogSOHItem, Log, TEXT("[BeginPlay] %s (ItemID: %s)"), *GetName(), *itemID.ToString());

    // 만약 레벨에 미리 배치해두고 ID를 적어놨다면, 시작하자마자 초기화 진행
    if (!itemID.IsNone())
    {
        InitItem(itemID);
    }
    
    if (!CutscenePlayer)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), CutscenePlayerActorTag, Found);
        if (Found.Num() > 0)
        {
            CutscenePlayer = Cast<ACutscenePlayerBase>(Found[0]);
        }
    }
}

void ASOHBaseItem::InitItem(FName newItemID)
{
    // 1. ID 저장
    itemID = newItemID;
    
    // 2. 게임 인스턴스를 통해 ItemManager 찾기
    UGameInstance* gameInst = GetGameInstance();
    USOHItemManager* itemManager = gameInst ? gameInst->GetSubsystem<USOHItemManager>() : nullptr;

    if (itemManager)
    {
        // 3. 매니저에게 "이 ID에 해당하는 정보 좀 줘" 요청
        FSOHItemTableRow* itemData = itemManager->GetItemDataByID(itemID);

        // 4. 정보가 있고, 메쉬 데이터가 있다면 내 몸(itemMesh)을 갈아입기
        if (itemData)
        {
            if (itemData->mesh)
            {
                itemMesh->SetStaticMesh(itemData->mesh);
                UE_LOG(LogSOHItem, Log, TEXT("[InitItem] Mesh Set Successfully for %s"), *itemID.ToString());

                // (선택사항) 아이템 이름으로 액터 이름 바꾸기 (디버깅용)
                #if WITH_EDITOR
                    SetActorLabel(itemData->itemName.ToString());
                #endif
            }
            else
            {
                UE_LOG(LogSOHItem, Log, TEXT("[InitItem] Data Found, but Mesh is NULL for ID: %s"), *itemID.ToString());
            }
        }
        else
        {
            UE_LOG(LogSOHItem, Log, TEXT("[InitItem] Init Failed: Data Table has no row for ID: %s"), *itemID.ToString());
        }
    }
    else
    {
        UE_LOG(LogSOHItem, Log, TEXT("[InitItem] CRITICAL: SOHItemManager Subsystem NOT Found!"));
    }
}

// 인터페이스 함수 구현
void ASOHBaseItem::Interact_Implementation(AActor* Caller)
{
    Super::Interact_Implementation(Caller);
    UE_LOG(LogSOHItem, Log, TEXT("[Interact] Called by Actor: %s"),
        Caller ? *Caller->GetName() : TEXT("NULL"));

    if (!Caller) return;

    USOHInventoryComponent* InventoryComp = Caller->FindComponentByClass<USOHInventoryComponent>();
    if (!InventoryComp)
    {
        UE_LOG(LogSOHItem, Log, TEXT("[Interact] Interactor has no SOHInventoryComponent!"));
        return;
    }

    UE_LOG(LogSOHItem, Log, TEXT("[Interact] Inventory Component Found. Trying to Add..."));

    const bool bSuccess = InventoryComp->AddToInventory(itemID, 1);
    if (!bSuccess)
    {
        UE_LOG(LogSOHItem, Log, TEXT("[Interact] Inventory Full or Error!"));
        return;
    }

    // “줍기 성공” 처리
    bCollected = true;

    TryTriggerItemCutscene();

    if (USOHGameInstance* GI = GetGameInstance<USOHGameInstance>())
    {
        // 태그 처리 (있으면)
        if (ItemConditionTag.IsValid())
        {
            GI->CompleteCondition(ItemConditionTag);
            UE_LOG(LogSOHItem, Log, TEXT("[Interact] Condition Sent From Item: %s"), *ItemConditionTag.ToString());
        }

        // 태그 유무 상관없이 저장은 무조건 호출
        GI->SaveGameData();
    }

    // 메시지 출력
    UGameInstance* gameInst = GetGameInstance();
    USOHItemManager* itemManager = gameInst ? gameInst->GetSubsystem<USOHItemManager>() : nullptr;

    FText ItemName = FText::FromName(itemID);
    if (itemManager)
    {
        if (FSOHItemTableRow* itemData = itemManager->GetItemDataByID(itemID))
        {
            ItemName = itemData->itemName;
        }
    }

    if (USOHMessageManager* MessageMgr = Caller->FindComponentByClass<USOHMessageManager>())
    {
        FText Msg = FText::Format(FText::FromString(TEXT("{0}을(를) 획득했다.")), ItemName);
        MessageMgr->ShowMessageText(Msg, 1.5f);
    }

    // 저장 후 제거
    Destroy();
}

void ASOHBaseItem::TryTriggerItemCutscene()
{
    // ✅ 흰색 로그
    UE_LOG(LogSOHItem, Log, TEXT("[Cutscene] TryTriggerItemCutscene CALLED"));

    if (!IsValid(CutscenePlayer))
    {
        UE_LOG(LogSOHItem, Log, TEXT("[Cutscene] CutscenePlayer INVALID"));
        return;
    }

    USOHGameInstance* GI = GetGameInstance<USOHGameInstance>();
    if (!GI)
    {
        UE_LOG(LogSOHItem, Log, TEXT("[Cutscene] GameInstance INVALID"));
        return;
    }

    if (CheckTag.IsValid() && GI->HasCondition(CheckTag))
    {
        UE_LOG(LogSOHItem, Log, TEXT("[Cutscene] Already Played → Skip"));
        return;
    }

    UE_LOG(LogSOHItem, Log, TEXT("[Cutscene] Execute_PlayCutscene"));
    CutscenePlayer->PlayCutscene();
}

void ASOHBaseItem::SaveState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone()) return;

    FWorldStateData& Data = SaveData->WorldStateMap.FindOrAdd(WorldStateID);
    Data.bIsCollected = bCollected;
}

void ASOHBaseItem::LoadState_Implementation(USOHSaveGame* SaveData)
{
    if (!SaveData || WorldStateID.IsNone()) return;

    if (FWorldStateData* Data = SaveData->WorldStateMap.Find(WorldStateID))
    {
        if (Data->bIsCollected)
        {
            Destroy();
        }
    }
}