#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h" // 탁완님 헤더
#include "SOHLockActor.generated.h"

class ASOHSlidingDoor;

/**
 * 특정 열쇠가 있어야만 상호작용에 성공하는 자물쇠 액터
 */
UCLASS()
class SOH_API ASOHLockActor : public ASOHInteractableActor
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
	ASOHSlidingDoor* TargetDoor;
	
protected:
	// 자물쇠 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock System")
	UStaticMeshComponent* LockMesh;
};