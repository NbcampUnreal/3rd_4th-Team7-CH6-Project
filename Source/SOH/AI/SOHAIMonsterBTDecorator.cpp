#include "SOHAIMonsterBTDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"

USOHAIMonsterBTDecorator::USOHAIMonsterBTDecorator()
{
	NodeName = TEXT("Check Player In Range");
}

bool USOHAIMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;

	const bool bPlayerInRange = BB->GetValueAsBool(TEXT("PlayerInRange"));
	return bPlayerInRange;
}
