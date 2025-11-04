#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SOHAttackTaskNode.generated.h"

UCLASS()
class SOH_API USOHAttackTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USOHAttackTaskNode();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
