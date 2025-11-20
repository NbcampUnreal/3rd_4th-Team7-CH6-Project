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
    
public:	
	// 생성자
	ASOHItem();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

public:	
	/**
	 * 아이템을 초기화하는 함수
	 * Manager에게 ID를 주면, 데이터 테이블에서 정보를 읽어와 외형 등을 설정함
	 */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitItem(FName newItemID);

	// 아이템 ID (데이터 테이블의 Row Name)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName itemID;

protected:
	// 아이템의 3D 외형을 담당하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* itemMesh;
};