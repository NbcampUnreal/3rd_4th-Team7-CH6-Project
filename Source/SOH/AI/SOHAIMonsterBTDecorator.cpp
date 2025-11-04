#include "SOHAIMonsterBTDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"

USOHAIMonsterBTDecorator::USOHAIMonsterBTDecorator()
{
	NodeName = TEXT("Check Player In Range");
}


bool USOHAIMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        const bool bSensed = BB->GetValueAsBool(TEXT("PlayerInRange"));
        const bool bInAttackRange = BB->GetValueAsBool(TEXT("AttackRange"));
        return bSensed && bInAttackRange;
    }

    return false;
}
