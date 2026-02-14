#include "SOHInventoryComponent.h"
#include "SOHItemManager.h"        // 매니저 접근용
#include "Kismet/GameplayStatics.h" // GameInstance 접근용
#include "Character/SOHPlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogSOHInventory, Log, All);

USOHInventoryComponent::USOHInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // 인벤토리는 틱이 필요 없습니다 (최적화)
}

void USOHInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool USOHInventoryComponent::AddToInventory(FName newItemID, int32 newQuantity)
{
    // 0. 유효하지 않은 개수면 무시
    if (newQuantity <= 0) return false;

    // 1. ItemManager를 불러와서 존재하는 아이템인지 확인 (검증)
    UGameInstance* gameInst = UGameplayStatics::GetGameInstance(GetWorld());
    USOHItemManager* itemManager = gameInst ? gameInst->GetSubsystem<USOHItemManager>() : nullptr;

    if (itemManager)
    {
       // 데이터 테이블에 없는 아이템 ID라면 추가하지 않음
       FSOHItemTableRow* itemData = itemManager->GetItemDataByID(newItemID);
       if (itemData == nullptr)
       {
          // ✅ 흰색 로그 (LogSOHInventory)
          UE_LOG(LogSOHInventory, Log, TEXT("AddToInventory Failed: Invalid Item ID (%s)"), *newItemID.ToString());
          return false;
       }
    }

    // 2. 이미 가지고 있는 아이템인지 확인 (있으면 개수만 증가)
    for (FSOHInventoryItem& slot : inventoryContents)
    {
       if (slot.itemID == newItemID)
       {
          slot.quantity += newQuantity;

          if (ItemPickupSound)
             UGameplayStatics::PlaySound2D(this, ItemPickupSound);

          UE_LOG(LogSOHInventory, Log, TEXT("Item Stacked: %s, New Qty: %d"), *newItemID.ToString(), slot.quantity);
          return true;
       }
    }

    // 3. 없으면 새로 추가
    inventoryContents.Add(FSOHInventoryItem(newItemID, newQuantity));

    if (ItemPickupSound)
       UGameplayStatics::PlaySound2D(this, ItemPickupSound);

    UE_LOG(LogSOHInventory, Log, TEXT("Item Added New Slot: %s, Qty: %d"), *newItemID.ToString(), newQuantity);
    
    return true;
}

bool USOHInventoryComponent::UseItem(FName ItemID)
{
    // 1. 수량 체크
    int32 Quantity = GetItemQuantity(ItemID);
    if (Quantity <= 0) return false;

    // 2. 데이터 가져오기
    UGameInstance* GameInst = UGameplayStatics::GetGameInstance(GetWorld());
    USOHItemManager* ItemManager = GameInst ? GameInst->GetSubsystem<USOHItemManager>() : nullptr;
    if (!ItemManager) return false;

    FSOHItemTableRow* ItemData = ItemManager->GetItemDataByID(ItemID);
    if (!ItemData) return false;

    // 3. 일단 '소비(Consumable)' 타입인지 먼저 확인 (선택사항이지만 권장)
    if (ItemData->itemType != ESOHItemType::Consumable)
    {
       UE_LOG(LogSOHInventory, Log, TEXT("이 아이템은 사용할 수 없는 타입입니다: %s"), *ItemID.ToString());
       return false;
    }

    bool bUsedSuccessfully = false;

    // 4. 태그 검사: "Health" 태그가 포함되어 있는가?
    if (ItemData->itemTags.Contains(FName("Health")))
    {
       // 컴포넌트의 주인(Player)을 찾기
       AActor* Owner = GetOwner();
       ASOHPlayerCharacter* PlayerCharacter = Cast<ASOHPlayerCharacter>(Owner);
       
       if (PlayerCharacter)
       {
          // [체력 회복 로직]
          PlayerCharacter->Heal(ItemData->value);
          
          UE_LOG(LogSOHInventory, Log, TEXT("[Inventory] 체력 회복 아이템 사용! (%s) -> 회복량: %.1f"), 
             *ItemData->itemName.ToString(), ItemData->value);
            
          bUsedSuccessfully = true;
       }
    }
    /*
    // 5. 마나 아이템 예시
    else if (ItemData->itemTags.Contains(FName("Mana")))
    {
       UE_LOG(LogSOHInventory, Log, TEXT("[Inventory] 마나 회복 아이템 사용!"));
       bUsedSuccessfully = true;
    }
    */
    else
    {
       UE_LOG(LogSOHInventory, Log, TEXT("사용 가능한 태그(Health 등)가 없는 아이템입니다: %s"), *ItemID.ToString());
    }

    // 6. 사용 성공 시 차감
    if (bUsedSuccessfully)
    {
       ConsumeItem(ItemID, 1);
       return true;
    }
    
    return false;
}

bool USOHInventoryComponent::ConsumeItem(FName ItemID, int32 Count)
{
    if (Count <= 0) return false;

    for (int32 i = 0; i < inventoryContents.Num(); i++)
    {
       FSOHInventoryItem& Slot = inventoryContents[i];

       if (Slot.itemID == ItemID)
       {
          // 수량 부족
          if (Slot.quantity < Count) return false;

          Slot.quantity -= Count;

          // 0개 되면 슬롯 삭제
          if (Slot.quantity <= 0)
          {
             inventoryContents.RemoveAt(i);
             // 삭제 로그
             UE_LOG(LogSOHInventory, Log, TEXT("Item Removed Completely: %s"), *ItemID.ToString());
          }
          else
          {
             // 차감 로그
             UE_LOG(LogSOHInventory, Log, TEXT("Item Consumed: %s, Remaining: %d"), *ItemID.ToString(), Slot.quantity);
          }
          return true;
       }
    }
    return false; // 해당 아이템 없음
}

int32 USOHInventoryComponent::GetItemQuantity(FName targetItemID) const
{
    for (const FSOHInventoryItem& slot : inventoryContents)
    {
       if (slot.itemID == targetItemID)
       {
          return slot.quantity;
       }
    }
    return 0;
}

void USOHInventoryComponent::GetInventoryContents_BP(TArray<FSOHInventoryItem>& OutItems) const
{
    OutItems = inventoryContents;
}

void USOHInventoryComponent::LoadInventory(const TArray<FSOHInventoryItem>& SavedItems)
{
    inventoryContents = SavedItems;
}