#pragma once

#include "CoreMinimal.h"
#include "Interaction/SOHInteractableActor.h"
#include "GameMode/SOHSaveObjectInterface.h"
#include "GameMode/SOHSaveGame.h"
#include "SOHBust.generated.h"

// ȸ�� �̺�Ʈ ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBustRotated);

class UTimelineComponent;
class UCurveFloat;

UCLASS()
class SOH_API ASOHBust 
	: public ASOHInteractableActor
	, public ISOHSaveObjectInterface
{
	GENERATED_BODY()

public:
	ASOHBust();

	virtual void Interact_Implementation(AActor* Caller) override;

	// ���� �Ŵ����� ���� �� �־�� �ϹǷ� public
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Position = 0;

	// ���� �Ŵ������� ���ε���
	UPROPERTY(BlueprintAssignable)
	FBustRotated OnBustRotated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Puzzle")
	bool bIsLocked = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
	FName WorldStateID;
	
	virtual void SaveState_Implementation(USOHSaveGame* SaveData) override;
	virtual void LoadState_Implementation(USOHSaveGame* SaveData) override;

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
	// ������ ����/��ǥ ȸ�� ��
	FRotator StartRotation;
	FRotator TargetRotation;
};
