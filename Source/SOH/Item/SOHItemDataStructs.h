// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SOHItemDataStructs.generated.h"

/**
 * 아이템의 종류를 구분하는 열거형
 */
UENUM(BlueprintType)
enum class ESOHItemType : uint8
{
	None        UMETA(DisplayName = "None"),
	Consumable  UMETA(DisplayName = "Consumable"), // 포션, 음식 등
	Equip       UMETA(DisplayName = "Equip"),      // 무기, 장비 등
	Quest       UMETA(DisplayName = "Quest"),      // 퀘스트 아이템
	Puzzle      UMETA(DisplayName = "Puzzle")      // 퍼즐용 아이템
};

/**
 * 데이터 테이블의 한 행(Row)을 정의하는 구조체
 * FTableRowBase를 상속받아야 데이터 테이블로 사용할 수 있습니다.
 */
USTRUCT(BlueprintType)
struct FSOHItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 1. 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FText itemName;

	// 2. 아이템 타입 (위에서 만든 Enum 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	ESOHItemType itemType;

	// 3. 아이템 설명 (툴팁 등에 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FText description;

	// 4. 인벤토리 UI에 표시될 아이콘 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UTexture2D* icon;

	// 5. 월드에 떨어졌을 때 보일 3D 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UStaticMesh* mesh;

	// 6. 수치값 (회복량, 공격력 등 범용적으로 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	float value;
	
	// 7. 아이템 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	TArray<FName> itemTags;
};

// 인벤토리 슬롯 구조체 정의
USTRUCT(BlueprintType)
struct FSOHInventoryItem
{
	GENERATED_BODY()

public:
	// 아이템 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName itemID = NAME_None;
    
	// 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 quantity = 0;
    
	// 기본 생성자
	FSOHInventoryItem()
	{
		
	}

	// 값을 받는 생성자
	FSOHInventoryItem(FName inID, int32 inQuantity)
	{
		itemID = inID;
		quantity = inQuantity;
	}
};