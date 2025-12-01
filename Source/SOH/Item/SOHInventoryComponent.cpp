#include "SOHInventoryComponent.h"
#include "SOHItemManager.h"        // 매니저 접근용
#include "Kismet/GameplayStatics.h" // GameInstance 접근용

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
			UE_LOG(LogTemp, Warning, TEXT("AddToInventory Failed: Invalid Item ID (%s)"), *newItemID.ToString());
			return false;
		}
	}

	// 2. 이미 가지고 있는 아이템인지 확인 (있으면 개수만 증가)
	for (FSOHInventoryItem& slot : inventoryContents)
	{
		if (slot.itemID == newItemID)
		{
			slot.quantity += newQuantity;
			UE_LOG(LogTemp, Log, TEXT("Item Stacked: %s, New Qty: %d"), *newItemID.ToString(), slot.quantity);
			return true;
		}
	}

	// 3. 없으면 새로 추가
	inventoryContents.Add(FSOHInventoryItem(newItemID, newQuantity));
	UE_LOG(LogTemp, Log, TEXT("Item Added New Slot: %s, Qty: %d"), *newItemID.ToString(), newQuantity);
    
	return true;
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