#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SOHAIMonsterBTService.generated.h"

UCLASS()
class CH6_API USOHAIMonsterBTService : public UBTService
{
	GENERATED_BODY()


public:
	USOHAIMonsterBTService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
