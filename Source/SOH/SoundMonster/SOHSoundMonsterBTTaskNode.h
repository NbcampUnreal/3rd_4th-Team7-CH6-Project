#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SOHSoundMonsterBTTaskNode.generated.h"

UCLASS()
class SOH_API USOHSoundMonsterBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	USOHSoundMonsterBTTaskNode();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
