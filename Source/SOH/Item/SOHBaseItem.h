#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHItemDataStructs.h" // 데이터 구조체
#include "GameplayTagContainer.h"
#include "GameMode/SOHCutscenePlayerBase.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "SOHBaseItem.generated.h"

UCLASS()
class SOH_API ASOHBaseItem 
	: public ASOHInteractableActor
	, public ISOHSaveObjectInterface
{
	GENERATED_BODY()
    
public:
	ASOHBaseItem();

protected:
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Caller) override;

	//// 다른 액터가 이 아이템과 겹치기 시작할 때 호출됨 (테스트 용으로 겹쳐지면 아이템 삭제 필요없을 때 주석 또는 삭제)
	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
public:
	// 아이템 초기화 함수
	// Manager에게 ID를 주면, 데이터 테이블에서 정보를 읽어와 외형 등을 설정함
	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitItem(FName newItemID);

	// 아이템 ID (데이터 테이블의 Row Name)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName itemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameplayTag")
	FGameplayTag ItemConditionTag;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Cutscene")
	ACutscenePlayerBase* CutscenePlayer = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cutscene")
	FName CutscenePlayerActorTag = TEXT("CutscenePlayer");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	FGameplayTag CheckTag;
	
	void TryTriggerItemCutscene();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Save")
	FName WorldStateID;
	
	virtual void SaveState_Implementation(USOHSaveGame* SaveData) override;
	virtual void LoadState_Implementation(USOHSaveGame* SaveData) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
	bool bCollected = false;
	
protected:
	// 아이템의 3D 외형을 담당하는 컴포넌트 (Mesh) 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* itemMesh;
};