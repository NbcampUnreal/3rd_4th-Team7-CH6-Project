#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SOHItemDataStructs.h" // 인벤토리 아이템 구조체 사용을 위해 포함
#include "SOHInventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOH_API USOHInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USOHInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	
	// 아이템을 인벤토리에 추가하는 함수 (성공 시 true 반환)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddToInventory(FName newItemID, int32 newQuantity);

	// 아이템 제거 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(FName ItemID, int32 Count);

	// 현재 인벤토리 내용물 확인용 (디버깅 또는 UI용)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FSOHInventoryItem>& GetInventoryContents() const { return inventoryContents; }

	// 특정 아이템의 보유 개수 확인
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemQuantity(FName targetItemID) const;

	// 인벤 UI에 쓰임
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GetInventoryContents_BP(TArray<FSOHInventoryItem>& OutItems) const;
	
	// 로드 인벤토리
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LoadInventory(const TArray<FSOHInventoryItem>& SavedItems);

protected:
	// 실제 아이템들이 저장될 배열 (우리들의 가방)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FSOHInventoryItem> inventoryContents;
};