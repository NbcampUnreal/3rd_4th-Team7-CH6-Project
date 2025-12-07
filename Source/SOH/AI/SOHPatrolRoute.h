#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOHPatrolRoute.generated.h"

class ATargetPoint;

UCLASS()
class SOH_API ASOHPatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	ASOHPatrolRoute();

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI")
	TArray<ATargetPoint*> PatrolPoints;
};
