#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "Components/TimelineComponent.h"
#include "SOHBust.generated.h"

UCLASS()
class SOH_API ASOHBust : public ASOHInteractableActor
{
	GENERATED_BODY()

public:    
	ASOHBust();

	// 인터랙트 오버라이드
	virtual void Interact_Implementation(AActor* Caller) override;

	// 현재 Position (누적 회전)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Bust")
	int32 Position = 0;
protected:
	// BeginPlay 오버라이드
	virtual void BeginPlay() override;

	// 회전할 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;


	// 한 번 Position당 회전할 각도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bust")
	float YawPerPosition = 90.f;

private:
	// 시작 회전값 (타임라인 기준)
	UPROPERTY()
	FRotator StartRotation;

	// 타임라인 컴포넌트
	UPROPERTY()
	UTimelineComponent* RotationTimeline;

	// 타임라인에 사용할 플로트 커브
	UPROPERTY(EditAnywhere, Category = "Rotation")
	UCurveFloat* RotationCurve;

	// 타임라인 업데이트 델리게이트
	UFUNCTION()
	void HandleTimelineUpdate(float Alpha);

	// 타임라인 종료 후 호출
	UFUNCTION()
	void HandleTimelineFinished();
};
