#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SOHSpiderMonsterAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
struct FAIStimulus;

UCLASS()
class SOH_API ASOHSpiderMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASOHSpiderMonsterAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	void StartPatrol();

	void MoveToRandomPatrolLocation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	float PatrolInterval;

	FTimerHandle PatrolTimerHandle;

	FTimerHandle StopTimerHandle;
	bool bIsStopped;
};
