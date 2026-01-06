#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h" // 탁완님 헤더
#include "GameMode/SOHSaveObjectInterface.h"
#include "SOHLockActor.generated.h"

class ASOHOpenDoor;
class ASOHSlidingDoor;

/**
 * 특정 열쇠가 있어야만 상호작용에 성공하는 자물쇠 액터
 */
UCLASS()
class SOH_API ASOHLockActor 
	: public ASOHInteractableActor
	, public ISOHSaveObjectInterface
{
	GENERATED_BODY()
    
public:
	ASOHLockActor();

protected:
	// 상호작용(E키) 함수 오버라이드
	virtual void Interact_Implementation(AActor* Caller) override;

public:
	// 이 자물쇠를 여는 데 필요한 열쇠의 ID (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	FName RequiredKeyID;

	// 이 자물쇠를 열면 잠금이 해제될 문
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	TArray<ASOHSlidingDoor*> TargetSlidingDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	TArray<ASOHOpenDoor*> TargetOpenDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	TArray<ASOHLockActor*> LinkedLocks;
	
	UFUNCTION(BlueprintCallable, Category="Lock System")
	void UnlockByScript(AActor* Caller, bool bDestroyAfterUnlock = true);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
	FName WorldStateID;

	virtual void SaveState_Implementation(USOHSaveGame* SaveData) override;
	virtual void LoadState_Implementation(USOHSaveGame* SaveData) override;
	
	UPROPERTY()
	bool bUnlocked = false;
	
protected:
	// 자물쇠 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock System")
	UStaticMeshComponent* LockMesh;

	// 잠금해제 사운드
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* UnlockSound;
};