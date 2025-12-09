#include "Item/SOHBaseItem.h" // 경로 확인
#include "SOHItemManager.h"
#include "SOHInventoryComponent.h" // 인벤토리 컴포넌트
#include "Components/StaticMeshComponent.h"
#include "UI/SOHMessageManager.h"

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

    // BeginPlay 로그: 아이템이 월드에 생성되었음을 알림
    UE_LOG(LogTemp, Warning, TEXT("[SOHBaseItem] BeginPlay: %s (ItemID: %s)"), *GetName(), *itemID.ToString());

    // 만약 레벨에 미리 배치해두고 ID를 적어놨다면, 시작하자마자 초기화 진행
    if (!itemID.IsNone())
    {
        InitItem(itemID);
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("[SOHBaseItem] Error: ItemID is None at BeginPlay! Check Blueprint Defaults."));
    }
}

void ASOHBaseItem::InitItem(FName newItemID)
{
    // 1. ID 저장
    itemID = newItemID;
    // UE_LOG(LogTemp, Log, TEXT("[SOHBaseItem] InitItem Called. Requesting Data for ID: %s"), *itemID.ToString());

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
                UE_LOG(LogTemp, Log, TEXT("[SOHBaseItem] Mesh Set Successfully for %s"), *itemID.ToString());

                // (선택사항) 아이템 이름으로 액터 이름 바꾸기 (디버깅용)
                #if WITH_EDITOR
                    SetActorLabel(itemData->itemName.ToString());
                #endif
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[SOHBaseItem] Data Found, but Mesh is NULL for ID: %s"), *itemID.ToString());
            }
        }
        else
        {
            UE_LOG(LogTemp,Warning, TEXT("[SOHBaseItem] Init Failed: Data Table has no row for ID: %s"), *itemID.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SOHBaseItem] CRITICAL: SOHItemManager Subsystem NOT Found!"));
    }
}

// 인터페이스 함수 구현
void ASOHBaseItem::Interact_Implementation(AActor* Caller)
{
    // 부모의 로직(로그 출력)도 실행하고 싶다면 호출 (선택사항)
    Super::Interact_Implementation(Caller);

    // 상호작용 시도 로그
    UE_LOG(LogTemp, Warning, TEXT("[SOHBaseItem] Interact Called by Actor: %s"), Caller ? *Caller->GetName() : TEXT("NULL"));

    if (!Caller) return;

    // 1. 상호작용한 대상(Caller)에게 인벤토리 컴포넌트가 있는지 확인
    // Caller는 AActor 타입이므로 바로 컴포넌트 검색 가능
    USOHInventoryComponent* InventoryComp = Caller->FindComponentByClass<USOHInventoryComponent>();

    if (InventoryComp)
    {
        UE_LOG(LogTemp, Log, TEXT("[SOHBaseItem] Inventory Component Found on Caller. Trying to Add..."));
        
        // 2. 가방에 넣기 시도
        bool bSuccess = InventoryComp->AddToInventory(itemID, 1);

        if (bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("BaseItem: Picked up by %s -> ItemID: %s"), *Caller->GetName(), *itemID.ToString());

            UGameInstance* gameInst = GetGameInstance();
            USOHItemManager* itemManager = gameInst ? gameInst->GetSubsystem<USOHItemManager>() : nullptr;

            FText ItemName = FText::FromName(itemID);
            if (itemManager)
            {
                FSOHItemTableRow* itemData = itemManager->GetItemDataByID(itemID);
                if (itemData)
                {
                    ItemName = itemData->itemName;
                }
            }

            USOHMessageManager* MessageMgr = Caller->FindComponentByClass<USOHMessageManager>();
            if (MessageMgr)
            {
                FText Msg = FText::Format(
                    FText::FromString(TEXT("{0}을(를) 획득했다.")),
                    ItemName
                );
                MessageMgr->ShowMessageText(Msg, 1.5f);
            }

            Destroy();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Inventory Full or Error!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Interactor has no SOHInventoryComponent!"));
    }
}

// 테스트 코드
//void ASOHBaseItem::NotifyActorBeginOverlap(AActor* OtherActor)
//{
//    Super::NotifyActorBeginOverlap(OtherActor);
//
//    // 1. 부딪힌 대상(OtherActor)이 유효한지, 그리고 나 자신(this)이 아닌지 확인
//    if (OtherActor && OtherActor != this)
//    {
//        // 2. 부딪힌 대상에게 '인벤토리 컴포넌트'가 있는지 찾아본다.
//        // (FindComponentByClass는 액터에 붙은 컴포넌트를 검색해 줍니다)
//        USOHInventoryComponent* InventoryComp = OtherActor->FindComponentByClass<USOHInventoryComponent>();
//
//        if (InventoryComp)
//        {
//            // 3. 인벤토리가 있다면, 아이템 추가 시도!
//            // (지금은 테스트니까 1개씩 추가한다고 가정)
//            bool bSuccess = InventoryComp->AddToInventory(itemID, 1);
//
//            if (bSuccess)
//            {
//                // 4. 추가에 성공했으면 로그를 남기고
//                UE_LOG(LogTemp, Log, TEXT("Item Picked Up: %s"), *itemID.ToString());
//
//                // 5. 이 아이템 액터는 세상에서 사라진다. (Destroy)
//                Destroy();
//            }
//        }
//    }
//}