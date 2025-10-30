#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SOHAIMonsterBTDecorator.generated.h"

UCLASS()
class SOH_API USOHAIMonsterBTDecorator : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	USOHAIMonsterBTDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
