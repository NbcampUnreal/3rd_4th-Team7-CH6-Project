#include "SOHItemManager.h"

USOHItemManager::USOHItemManager()
{
	// 생성자에서 데이터 테이블을 로드
	static ConstructorHelpers::FObjectFinder<UDataTable> dtAsset(TEXT("/Game/Item/DT_SOHItemData.DT_SOHItemData"));
    
	if (dtAsset.Succeeded())
	{
		itemDataTable = dtAsset.Object;
	}
}

void USOHItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
    
	// 로드 잘 됐는지 로그로 확인
	if (itemDataTable)
	{
		UE_LOG(LogTemp, Log, TEXT("SOHItemManager: Data Table Loaded Successfully!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SOHItemManager: Failed to Load Data Table!"));
	}
}

FSOHItemTableRow* USOHItemManager::GetItemDataByID(FName key)
{
	if (!itemDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable is null!"));
		return nullptr;
	}

	// FindRow는 템플릿 함수라 구조체 타입을 <> 안에 명시해야 함
	// ContextString은 에러 났을 때 로그에 찍힐 힌트 (빈칸이어도 됨)
	static const FString contextString(TEXT("SOHItemManager::GetItemDataByID"));
    
	return itemDataTable->FindRow<FSOHItemTableRow>(key, contextString);
}