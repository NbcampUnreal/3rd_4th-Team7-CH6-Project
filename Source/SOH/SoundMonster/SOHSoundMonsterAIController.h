#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SOHSoundMonsterAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
struct FAIStimulus;

UCLASS()
class SOH_API ASOHSoundMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASOHSoundMonsterAIController();

	virtual void OnPossess(APawn* InPawn) override;

	UBlackboardComponent* GetBlackboard() const { return BlackboardComp; }

	static const FName Key_PlayerActor;
	static const FName Key_PlayerInRange;
	static const FName Key_PatrolTarget;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
