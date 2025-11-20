#include "SOHSoundMonsterBTDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SOHSoundMonsterAIController.h"

USOHSoundMonsterBTDecorator::USOHSoundMonsterBTDecorator()
{
	NodeName = TEXT("Check Player In Range (SoundMonster)");
}

bool USOHSoundMonsterBTDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		const bool bSensed = BB->GetValueAsBool(ASOHSoundMonsterAIController::Key_PlayerInRange);
		return bSensed;
	}

	return false;
}