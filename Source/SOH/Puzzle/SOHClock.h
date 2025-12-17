// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "SOHClock.generated.h"

/**
 * 
 */
UCLASS()
class SOH_API ASOHClock : public ASOHInteractableActor
{
	GENERATED_BODY()

public:
	ASOHClock();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ▼ 분침 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MinuteHand;

	// ▼ 드래그 중인지 여부
	bool bIsDragging = false;

	// ▼ 이전 프레임에서의 드래그각
	float StartDragAngle = 0.0f;

	// ▼ 최종 회전 값(Z축)
	float CurrentAngle = 0.0f;

	// ▼ 입력 함수
	void OnMouseDown();
	void OnMouseUp();

	// ▼ Helper
	float GetMouseAngle();
};
