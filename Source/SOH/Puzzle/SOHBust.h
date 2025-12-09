#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHBust.generated.h"

// 회전 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBustRotated);

class UTimelineComponent;
class UCurveFloat;

UCLASS()
class SOH_API ASOHBust : public ASOHInteractableActor
{
	GENERATED_BODY()

public:
	ASOHBust();

	virtual void Interact_Implementation(AActor* Caller) override;

	// 퍼즐 매니저가 읽을 수 있어야 하므로 public
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Position = 0;

	// 퍼즐 매니저에서 바인딩함
	UPROPERTY(BlueprintAssignable)
	FBustRotated OnBustRotated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Puzzle")
	bool bIsLocked = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY()
	UTimelineComponent* RotationTimeline;

	UPROPERTY(EditAnywhere)
	UCurveFloat* RotationCurve;

	UFUNCTION()
	void HandleTimelineUpdate(float Value);

	UFUNCTION()
	void HandleTimelineFinished();

	float YawPerPosition = 90.f;

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* RotateSound;

private:
	// 보간용 시작/목표 회전 값
	FRotator StartRotation;
	FRotator TargetRotation;
};
