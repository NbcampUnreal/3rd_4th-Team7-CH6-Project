#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SOHAIMonsterController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class AActor;
class ACSMonsterCharacter;

UCLASS()
class SOH_API ASOHAIMonsterController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASOHAIMonsterController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComp;
};
