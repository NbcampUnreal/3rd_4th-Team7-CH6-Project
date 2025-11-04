#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHAIMonster.generated.h"

class ATargetPoint;

UCLASS()
class SOH_API ASOHAIMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHAIMonster();


protected:
	virtual void BeginPlay() override;


public:
	/** 순찰 지점 배열 (에디터에서 배치 후 할당) */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<ATargetPoint*> PatrolTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChaseSpeed;

	// 시야
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float SightRadius;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float LoseSightRadius;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Sight")
	float PeripheralVisionAngle;


	//청각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception|Hearing")
	float HearingRange;

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void SetMoveSpeed(float NewSpeed);
};