#pragma once

#include "CoreMinimal.h"
#include "Item/SOHBaseItem.h" // 부모 클래스 헤더
#include "SOHItemDataStructs.h" // 데이터 구조체
#include "SOHItem.generated.h"

/**
 * 실제 게임 월드에 스폰되거나 배치되는 구체적인 아이템 클래스
 * SOHBaseItem(Master)을 상속받아 상호작용 기능을 물려받습니다.
 */
UCLASS()
class SOH_API ASOHItem : public ASOHBaseItem
{
	GENERATED_BODY()
};