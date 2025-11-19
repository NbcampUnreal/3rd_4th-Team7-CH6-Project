#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "SOHItemDataStructs.h"
#include "SOHItemManager.generated.h"

UCLASS()
class SOH_API USOHItemManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USOHItemManager();

	// 서브시스템 초기화 함수 (게임 시작 시 호출됨)
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ID(RowName)를 주면 아이템 데이터 포인터를 반환하는 함수
	FSOHItemTableRow* GetItemDataByID(FName key);

protected:
	// 우리가 만든 데이터 테이블을 저장할 변수
	UPROPERTY()
	UDataTable* itemDataTable;
};