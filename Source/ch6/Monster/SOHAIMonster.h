#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SOHAIMonster.generated.h"

class ATargetPoint;

UCLASS()
class CH6_API ASOHAIMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHAIMonster();

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<ATargetPoint*> PatrolTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float PatrolSpeed;
};
