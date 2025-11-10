#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SOHAIMonsterController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class SOH_API ASOHAIMonsterController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASOHAIMonsterController();

	void SetDetectOnlyPlayer();
	void RestoreDetectAll();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY()
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	static const FName Key_PlayerActor;
	static const FName Key_PlayerInRange;
	static const FName Key_PatrolTarget;

	static const FName Key_LastKnownLocation;
	static const FName Key_SearchPoint;
	static const FName Key_SearchUntilTime;
};
