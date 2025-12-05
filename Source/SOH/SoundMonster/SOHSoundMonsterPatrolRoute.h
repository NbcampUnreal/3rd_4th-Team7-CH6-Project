#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHSoundMonsterPatrolRoute.generated.h"

class ATargetPoint;

UCLASS()
class SOH_API ASOHSoundMonsterPatrolRoute : public AActor
{
	GENERATED_BODY()

public:
	ASOHSoundMonsterPatrolRoute();

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI")
	TArray<ATargetPoint*> PatrolPoints;
};
